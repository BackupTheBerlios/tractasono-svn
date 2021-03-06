#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <string.h>

GST_DEBUG_CATEGORY (seek_debug);
#define GST_CAT_DEFAULT (seek_debug)

static GList *seekable_pads = NULL;
static GList *rate_pads = NULL;
static GList *seekable_elements = NULL;

static gboolean accurate_seek = FALSE;
static gboolean keyframe_seek = FALSE;
static gboolean loop_seek = FALSE;
static gboolean flush_seek = TRUE;
static gboolean scrub = FALSE;

static GstElement *pipeline;
static gint64 position;
static gint64 duration;
static GtkAdjustment *adjustment;
static GtkWidget *hscale;
static gboolean stats = FALSE;
static gboolean elem_seek = FALSE;
static gboolean verbose = FALSE;

static GstState state;
static guint update_id = 0;
static guint seek_timeout_id = 0;
static gulong changed_id;

//#define SOURCE "filesrc"
#define SOURCE "gnomevfssrc"
#define ASINK "alsasink"
//#define ASINK "osssink"
#define VSINK "xvimagesink"
//#define VSINK "sdlvideosink"
//#define VSINK "ximagesink"
//#define VSINK "aasink"
//#define VSINK "cacasink"

//#define UPDATE_INTERVAL 500
//#define UPDATE_INTERVAL 100
#define UPDATE_INTERVAL 10

/* number of milliseconds to play for after a seek */
#define SCRUB_TIME 100

/* seek timeout */
#define SEEK_TIMEOUT 40 * GST_MSECOND

#define THREAD
#define PAD_SEEK

typedef struct
{
  const gchar *padname;
  GstPad *target;
  GstElement *bin;
}
dyn_link;

static GstElement *
gst_element_factory_make_or_warn (gchar * type, gchar * name)
{
  GstElement *element = gst_element_factory_make (type, name);

  if (!element) {
    g_warning ("Failed to create element %s of type %s", name, type);
  }

  return element;
}

static void
dynamic_link (GstPadTemplate * templ, GstPad * newpad, gpointer data)
{
  gchar *padname;
  dyn_link *connect = (dyn_link *) data;

  padname = gst_pad_get_name (newpad);

  if (connect->padname == NULL || !strcmp (padname, connect->padname)) {
    if (connect->bin)
      gst_bin_add (GST_BIN (pipeline), connect->bin);
    gst_pad_link (newpad, connect->target);

    //seekable_pads = g_list_prepend (seekable_pads, newpad);
    rate_pads = g_list_prepend (rate_pads, newpad);
  }
  g_free (padname);
}

static void
setup_dynamic_link (GstElement * element, const gchar * padname,
    GstPad * target, GstElement * bin)
{
  dyn_link *connect;

  connect = g_new0 (dyn_link, 1);
  connect->padname = g_strdup (padname);
  connect->target = target;
  connect->bin = bin;

  g_signal_connect (G_OBJECT (element), "pad-added", G_CALLBACK (dynamic_link),
      connect);
}

static GstElement *
make_mod_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *decoder, *audiosink;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  decoder = gst_element_factory_make_or_warn ("modplug", "decoder");
  audiosink = gst_element_factory_make_or_warn (ASINK, "sink");
  //g_object_set (G_OBJECT (audiosink), "sync", FALSE, NULL);

  g_object_set (G_OBJECT (src), "location", location, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), decoder);
  gst_bin_add (GST_BIN (pipeline), audiosink);

  gst_element_link (src, decoder);
  gst_element_link (decoder, audiosink);

  seekable = gst_element_get_pad (decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));

  return pipeline;
}

static GstElement *
make_dv_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *demux, *decoder, *audiosink, *videosink;
  GstElement *a_queue, *v_queue;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  demux = gst_element_factory_make_or_warn ("dvdemux", "demuxer");
  v_queue = gst_element_factory_make_or_warn ("queue", "v_queue");
  decoder = gst_element_factory_make_or_warn ("ffdec_dvvideo", "decoder");
  videosink = gst_element_factory_make_or_warn (VSINK, "v_sink");
  a_queue = gst_element_factory_make_or_warn ("queue", "a_queue");
  audiosink = gst_element_factory_make_or_warn ("alsasink", "a_sink");

  g_object_set (G_OBJECT (src), "location", location, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_bin_add (GST_BIN (pipeline), a_queue);
  gst_bin_add (GST_BIN (pipeline), audiosink);
  gst_bin_add (GST_BIN (pipeline), v_queue);
  gst_bin_add (GST_BIN (pipeline), decoder);
  gst_bin_add (GST_BIN (pipeline), videosink);

  gst_element_link (src, demux);
  gst_element_link (a_queue, audiosink);
  gst_element_link (v_queue, decoder);
  gst_element_link (decoder, videosink);

  setup_dynamic_link (demux, "video", gst_element_get_pad (v_queue, "sink"),
      NULL);
  setup_dynamic_link (demux, "audio", gst_element_get_pad (a_queue, "sink"),
      NULL);

  seekable = gst_element_get_pad (decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);

  return pipeline;
}

static GstElement *
make_wav_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *decoder, *audiosink;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  decoder = gst_element_factory_make_or_warn ("wavparse", "decoder");
  audiosink = gst_element_factory_make_or_warn (ASINK, "sink");

  g_object_set (G_OBJECT (src), "location", location, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), decoder);
  gst_bin_add (GST_BIN (pipeline), audiosink);

  gst_element_link (src, decoder);

  setup_dynamic_link (decoder, "src", gst_element_get_pad (audiosink, "sink"),
      NULL);

  seekable_elements = g_list_prepend (seekable_elements, audiosink);

  /* force element seeking on this pipeline */
  elem_seek = TRUE;

  return pipeline;
}

static GstElement *
make_flac_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *decoder, *audiosink;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  decoder = gst_element_factory_make_or_warn ("flacdec", "decoder");
  audiosink = gst_element_factory_make_or_warn (ASINK, "sink");
  g_object_set (G_OBJECT (audiosink), "sync", FALSE, NULL);

  g_object_set (G_OBJECT (src), "location", location, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), decoder);
  gst_bin_add (GST_BIN (pipeline), audiosink);

  gst_element_link (src, decoder);
  gst_element_link (decoder, audiosink);

  seekable = gst_element_get_pad (decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));

  return pipeline;
}

static GstElement *
make_sid_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *decoder, *audiosink;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  decoder = gst_element_factory_make_or_warn ("siddec", "decoder");
  audiosink = gst_element_factory_make_or_warn (ASINK, "sink");
  //g_object_set (G_OBJECT (audiosink), "sync", FALSE, NULL);

  g_object_set (G_OBJECT (src), "location", location, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), decoder);
  gst_bin_add (GST_BIN (pipeline), audiosink);

  gst_element_link (src, decoder);
  gst_element_link (decoder, audiosink);

  seekable = gst_element_get_pad (decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));

  return pipeline;
}

static GstElement *
make_parse_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *parser, *fakesink;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  parser = gst_element_factory_make_or_warn ("mpegparse", "parse");
  fakesink = gst_element_factory_make_or_warn ("fakesink", "sink");
  g_object_set (G_OBJECT (fakesink), "silent", TRUE, NULL);
  g_object_set (G_OBJECT (fakesink), "sync", TRUE, NULL);

  g_object_set (G_OBJECT (src), "location", location, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), parser);
  gst_bin_add (GST_BIN (pipeline), fakesink);

  gst_element_link (src, parser);
  gst_element_link (parser, fakesink);

  seekable = gst_element_get_pad (parser, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (parser, "sink"));

  return pipeline;
}

static GstElement *
make_vorbis_pipeline (const gchar * location)
{
  GstElement *pipeline, *audio_bin;
  GstElement *src, *demux, *decoder, *convert, *audiosink;
  GstPad *pad, *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  demux = gst_element_factory_make_or_warn ("oggdemux", "demux");
  decoder = gst_element_factory_make_or_warn ("vorbisdec", "decoder");
  convert = gst_element_factory_make_or_warn ("audioconvert", "convert");
  audiosink = gst_element_factory_make_or_warn (ASINK, "sink");
  g_object_set (G_OBJECT (audiosink), "sync", TRUE, NULL);

  g_object_set (G_OBJECT (src), "location", location, NULL);

  audio_bin = gst_bin_new ("a_decoder_bin");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_bin_add (GST_BIN (audio_bin), decoder);
  gst_bin_add (GST_BIN (audio_bin), convert);
  gst_bin_add (GST_BIN (audio_bin), audiosink);
  gst_bin_add (GST_BIN (pipeline), audio_bin);

  gst_element_link (src, demux);
  gst_element_link (decoder, convert);
  gst_element_link (convert, audiosink);

  pad = gst_element_get_pad (decoder, "sink");
  gst_element_add_pad (audio_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, NULL, gst_element_get_pad (audio_bin, "sink"),
      NULL);

  seekable = gst_element_get_pad (decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));

  return pipeline;
}

static GstElement *
make_theora_pipeline (const gchar * location)
{
  GstElement *pipeline, *video_bin;
  GstElement *src, *demux, *decoder, *convert, *videosink;
  GstPad *pad, *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  demux = gst_element_factory_make_or_warn ("oggdemux", "demux");
  decoder = gst_element_factory_make_or_warn ("theoradec", "decoder");
  convert = gst_element_factory_make_or_warn ("ffmpegcolorspace", "convert");
  videosink = gst_element_factory_make_or_warn (VSINK, "sink");

  g_object_set (G_OBJECT (src), "location", location, NULL);

  video_bin = gst_bin_new ("v_decoder_bin");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_bin_add (GST_BIN (video_bin), decoder);
  gst_bin_add (GST_BIN (video_bin), convert);
  gst_bin_add (GST_BIN (video_bin), videosink);
  gst_bin_add (GST_BIN (pipeline), video_bin);

  gst_element_link (src, demux);
  gst_element_link (decoder, convert);
  gst_element_link (convert, videosink);

  pad = gst_element_get_pad (decoder, "sink");
  gst_element_add_pad (video_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, NULL, gst_element_get_pad (video_bin, "sink"),
      NULL);

  seekable = gst_element_get_pad (decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));

  return pipeline;
}

static GstElement *
make_vorbis_theora_pipeline (const gchar * location)
{
  GstElement *pipeline, *audio_bin, *video_bin;
  GstElement *src, *demux, *a_decoder, *a_convert, *v_decoder, *v_convert;
  GstElement *audiosink, *videosink;
  GstElement *a_queue, *v_queue, *v_scale;
  GstPad *seekable;
  GstPad *pad;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  g_object_set (G_OBJECT (src), "location", location, NULL);

  demux = gst_element_factory_make_or_warn ("oggdemux", "demux");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_element_link (src, demux);

  audio_bin = gst_bin_new ("a_decoder_bin");
  a_queue = gst_element_factory_make_or_warn ("queue", "a_queue");
  a_decoder = gst_element_factory_make_or_warn ("vorbisdec", "a_dec");
  a_convert = gst_element_factory_make_or_warn ("audioconvert", "a_convert");
  audiosink = gst_element_factory_make_or_warn (ASINK, "a_sink");

  gst_bin_add (GST_BIN (pipeline), audio_bin);

  gst_bin_add (GST_BIN (audio_bin), a_queue);
  gst_bin_add (GST_BIN (audio_bin), a_decoder);
  gst_bin_add (GST_BIN (audio_bin), a_convert);
  gst_bin_add (GST_BIN (audio_bin), audiosink);

  gst_element_link (a_queue, a_decoder);
  gst_element_link (a_decoder, a_convert);
  gst_element_link (a_convert, audiosink);

  pad = gst_element_get_pad (a_queue, "sink");
  gst_element_add_pad (audio_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, NULL, gst_element_get_pad (audio_bin, "sink"),
      NULL);

  video_bin = gst_bin_new ("v_decoder_bin");
  v_queue = gst_element_factory_make_or_warn ("queue", "v_queue");
  v_decoder = gst_element_factory_make_or_warn ("theoradec", "v_dec");
  v_convert =
      gst_element_factory_make_or_warn ("ffmpegcolorspace", "v_convert");
  v_scale = gst_element_factory_make_or_warn ("videoscale", "v_scale");
  videosink = gst_element_factory_make_or_warn (VSINK, "v_sink");

  gst_bin_add (GST_BIN (pipeline), video_bin);

  gst_bin_add (GST_BIN (video_bin), v_queue);
  gst_bin_add (GST_BIN (video_bin), v_decoder);
  gst_bin_add (GST_BIN (video_bin), v_convert);
  gst_bin_add (GST_BIN (video_bin), v_scale);
  gst_bin_add (GST_BIN (video_bin), videosink);

  gst_element_link_many (v_queue, v_decoder, v_convert, v_scale, videosink,
      NULL);

  pad = gst_element_get_pad (v_queue, "sink");
  gst_element_add_pad (video_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, NULL, gst_element_get_pad (video_bin, "sink"),
      NULL);

  seekable = gst_element_get_pad (a_decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (a_decoder, "sink"));

  return pipeline;
}

static GstElement *
make_avi_msmpeg4v3_mp3_pipeline (const gchar * location)
{
  GstElement *pipeline, *audio_bin, *video_bin;
  GstElement *src, *demux, *a_decoder, *a_convert, *v_decoder, *v_convert;
  GstElement *audiosink, *videosink;
  GstElement *a_queue, *v_queue;
  GstPad *seekable, *pad;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  g_object_set (G_OBJECT (src), "location", location, NULL);

  demux = gst_element_factory_make_or_warn ("avidemux", "demux");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_element_link (src, demux);

  audio_bin = gst_bin_new ("a_decoder_bin");
  a_queue = gst_element_factory_make_or_warn ("queue", "a_queue");
  a_decoder = gst_element_factory_make_or_warn ("mad", "a_dec");
  a_convert = gst_element_factory_make_or_warn ("audioconvert", "a_convert");
  audiosink = gst_element_factory_make_or_warn (ASINK, "a_sink");

  gst_bin_add (GST_BIN (audio_bin), a_queue);
  gst_bin_add (GST_BIN (audio_bin), a_decoder);
  gst_bin_add (GST_BIN (audio_bin), a_convert);
  gst_bin_add (GST_BIN (audio_bin), audiosink);

  gst_element_link (a_queue, a_decoder);
  gst_element_link (a_decoder, a_convert);
  gst_element_link (a_convert, audiosink);

  gst_bin_add (GST_BIN (pipeline), audio_bin);

  pad = gst_element_get_pad (a_queue, "sink");
  gst_element_add_pad (audio_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, NULL, gst_element_get_pad (audio_bin, "sink"),
      NULL);

  video_bin = gst_bin_new ("v_decoder_bin");
  v_queue = gst_element_factory_make_or_warn ("queue", "v_queue");
  v_decoder = gst_element_factory_make_or_warn ("ffdec_msmpeg4", "v_dec");
  v_convert =
      gst_element_factory_make_or_warn ("ffmpegcolorspace", "v_convert");
  videosink = gst_element_factory_make_or_warn (VSINK, "v_sink");

  gst_bin_add (GST_BIN (video_bin), v_queue);
  gst_bin_add (GST_BIN (video_bin), v_decoder);
  gst_bin_add (GST_BIN (video_bin), v_convert);
  gst_bin_add (GST_BIN (video_bin), videosink);

  gst_element_link_many (v_queue, v_decoder, v_convert, videosink, NULL);

  gst_bin_add (GST_BIN (pipeline), video_bin);

  pad = gst_element_get_pad (v_queue, "sink");
  gst_element_add_pad (video_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, NULL, gst_element_get_pad (video_bin, "sink"),
      NULL);

  seekable = gst_element_get_pad (a_decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (a_decoder, "sink"));

  return pipeline;
}

static GstElement *
make_mp3_pipeline (const gchar * location)
{
  GstElement *pipeline;
  GstElement *src, *decoder, *osssink, *queue;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  decoder = gst_element_factory_make_or_warn ("mad", "dec");
  queue = gst_element_factory_make_or_warn ("queue", "queue");
  osssink = gst_element_factory_make_or_warn (ASINK, "sink");

  seekable_elements = g_list_prepend (seekable_elements, osssink);

  g_object_set (G_OBJECT (src), "location", location, NULL);
  //g_object_set (G_OBJECT (osssink), "fragment", 0x00180008, NULL);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), decoder);
  gst_bin_add (GST_BIN (pipeline), queue);
  gst_bin_add (GST_BIN (pipeline), osssink);

  gst_element_link (src, decoder);
  gst_element_link (decoder, queue);
  gst_element_link (queue, osssink);

  seekable = gst_element_get_pad (queue, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));

  return pipeline;
}

static GstElement *
make_avi_pipeline (const gchar * location)
{
  GstElement *pipeline, *audio_bin, *video_bin;
  GstElement *src, *demux, *a_decoder, *v_decoder, *audiosink, *videosink;
  GstElement *a_queue = NULL, *v_queue = NULL;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  g_object_set (G_OBJECT (src), "location", location, NULL);

  demux = gst_element_factory_make_or_warn ("avidemux", "demux");
  seekable_elements = g_list_prepend (seekable_elements, demux);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_element_link (src, demux);

  audio_bin = gst_bin_new ("a_decoder_bin");
  a_decoder = gst_element_factory_make_or_warn ("mad", "a_dec");
  audiosink = gst_element_factory_make_or_warn (ASINK, "a_sink");
  a_queue = gst_element_factory_make_or_warn ("queue", "a_queue");
  gst_element_link (a_decoder, a_queue);
  gst_element_link (a_queue, audiosink);
  gst_bin_add (GST_BIN (audio_bin), a_decoder);
  gst_bin_add (GST_BIN (audio_bin), a_queue);
  gst_bin_add (GST_BIN (audio_bin), audiosink);
  gst_element_set_state (audio_bin, GST_STATE_PAUSED);

  setup_dynamic_link (demux, "audio_00", gst_element_get_pad (a_decoder,
          "sink"), audio_bin);

  seekable = gst_element_get_pad (a_queue, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (a_decoder, "sink"));

  video_bin = gst_bin_new ("v_decoder_bin");
  v_decoder = gst_element_factory_make_or_warn ("ffmpegdecall", "v_dec");
  videosink = gst_element_factory_make_or_warn (VSINK, "v_sink");
  v_queue = gst_element_factory_make_or_warn ("queue", "v_queue");
  gst_element_link (v_decoder, v_queue);
  gst_element_link (v_queue, videosink);
  gst_bin_add (GST_BIN (video_bin), v_decoder);
  gst_bin_add (GST_BIN (video_bin), v_queue);
  gst_bin_add (GST_BIN (video_bin), videosink);

  gst_element_set_state (video_bin, GST_STATE_PAUSED);

  setup_dynamic_link (demux, "video_00", gst_element_get_pad (v_decoder,
          "sink"), video_bin);

  seekable = gst_element_get_pad (v_queue, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (v_decoder, "sink"));

  return pipeline;
}

static GstElement *
make_mpeg_pipeline (const gchar * location)
{
  GstElement *pipeline, *audio_bin, *video_bin;
  GstElement *src, *demux, *a_decoder, *v_decoder, *v_filter;
  GstElement *audiosink, *videosink;
  GstElement *a_queue, *v_queue;
  GstPad *seekable;
  GstPad *pad;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  g_object_set (G_OBJECT (src), "location", location, NULL);

  //demux = gst_element_factory_make_or_warn ("mpegdemux", "demux");
  demux = gst_element_factory_make_or_warn ("flupsdemux", "demux");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_element_link (src, demux);

  audio_bin = gst_bin_new ("a_decoder_bin");
  a_decoder = gst_element_factory_make_or_warn ("mad", "a_dec");
  a_queue = gst_element_factory_make_or_warn ("queue", "a_queue");
  audiosink = gst_element_factory_make_or_warn (ASINK, "a_sink");
  gst_bin_add (GST_BIN (audio_bin), a_decoder);
  gst_bin_add (GST_BIN (audio_bin), a_queue);
  gst_bin_add (GST_BIN (audio_bin), audiosink);

  gst_element_link (a_decoder, a_queue);
  gst_element_link (a_queue, audiosink);

  gst_bin_add (GST_BIN (pipeline), audio_bin);

  pad = gst_element_get_pad (a_decoder, "sink");
  gst_element_add_pad (audio_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, "audio_c0", gst_element_get_pad (audio_bin,
          "sink"), NULL);

  video_bin = gst_bin_new ("v_decoder_bin");
  v_decoder = gst_element_factory_make_or_warn ("mpeg2dec", "v_dec");
  v_queue = gst_element_factory_make_or_warn ("queue", "v_queue");
  v_filter = gst_element_factory_make_or_warn ("ffmpegcolorspace", "v_filter");
  videosink = gst_element_factory_make_or_warn (VSINK, "v_sink");

  gst_bin_add (GST_BIN (video_bin), v_decoder);
  gst_bin_add (GST_BIN (video_bin), v_queue);
  gst_bin_add (GST_BIN (video_bin), v_filter);
  gst_bin_add (GST_BIN (video_bin), videosink);

  gst_element_link (v_decoder, v_queue);
  gst_element_link (v_queue, v_filter);
  gst_element_link (v_filter, videosink);

  gst_bin_add (GST_BIN (pipeline), video_bin);

  pad = gst_element_get_pad (v_decoder, "sink");
  gst_element_add_pad (video_bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  setup_dynamic_link (demux, "video_e0", gst_element_get_pad (video_bin,
          "sink"), NULL);

  seekable = gst_element_get_pad (v_filter, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (v_decoder, "sink"));

  return pipeline;
}

static GstElement *
make_mpegnt_pipeline (const gchar * location)
{
  GstElement *pipeline, *audio_bin, *video_bin;
  GstElement *src, *demux, *a_decoder, *v_decoder, *v_filter;
  GstElement *audiosink, *videosink;
  GstElement *a_queue;
  GstPad *seekable;

  pipeline = gst_pipeline_new ("app");

  src = gst_element_factory_make_or_warn (SOURCE, "src");
  g_object_set (G_OBJECT (src), "location", location, NULL);

  demux = gst_element_factory_make_or_warn ("mpegdemux", "demux");
  //g_object_set (G_OBJECT (demux), "sync", TRUE, NULL);

  seekable_elements = g_list_prepend (seekable_elements, demux);

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), demux);
  gst_element_link (src, demux);

  audio_bin = gst_bin_new ("a_decoder_bin");
  a_decoder = gst_element_factory_make_or_warn ("mad", "a_dec");
  a_queue = gst_element_factory_make_or_warn ("queue", "a_queue");
  audiosink = gst_element_factory_make_or_warn (ASINK, "a_sink");
  //g_object_set (G_OBJECT (audiosink), "fragment", 0x00180008, NULL);
  g_object_set (G_OBJECT (audiosink), "sync", FALSE, NULL);
  gst_element_link (a_decoder, a_queue);
  gst_element_link (a_queue, audiosink);
  gst_bin_add (GST_BIN (audio_bin), a_decoder);
  gst_bin_add (GST_BIN (audio_bin), a_queue);
  gst_bin_add (GST_BIN (audio_bin), audiosink);

  setup_dynamic_link (demux, "audio_00", gst_element_get_pad (a_decoder,
          "sink"), audio_bin);

  seekable = gst_element_get_pad (a_queue, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (a_decoder, "sink"));

  video_bin = gst_bin_new ("v_decoder_bin");
  v_decoder = gst_element_factory_make_or_warn ("mpeg2dec", "v_dec");
  v_filter = gst_element_factory_make_or_warn ("ffmpegcolorspace", "v_filter");
  videosink = gst_element_factory_make_or_warn (VSINK, "v_sink");
  gst_element_link_many (v_decoder, v_filter, videosink, NULL);

  gst_bin_add_many (GST_BIN (video_bin), v_decoder, v_filter, videosink, NULL);

  setup_dynamic_link (demux, "video_00", gst_element_get_pad (v_decoder,
          "sink"), video_bin);

  seekable = gst_element_get_pad (v_decoder, "src");
  seekable_pads = g_list_prepend (seekable_pads, seekable);
  rate_pads = g_list_prepend (rate_pads, seekable);
  rate_pads =
      g_list_prepend (rate_pads, gst_element_get_pad (v_decoder, "sink"));

  return pipeline;
}

static GstElement *
make_playerbin_pipeline (const gchar * location)
{
  GstElement *player;

  player = gst_element_factory_make ("playbin", "player");
  g_assert (player);

  g_object_set (G_OBJECT (player), "uri", location, NULL);

  seekable_elements = g_list_prepend (seekable_elements, player);

  /* force element seeking on this pipeline */
  elem_seek = TRUE;

  return player;
}

static GstElement *
make_parselaunch_pipeline (const gchar * description)
{
  GstElement *pipeline;
  GError *error;

  pipeline = gst_parse_launch (description, &error);

  seekable_elements = g_list_prepend (seekable_elements, pipeline);

  elem_seek = TRUE;

  return pipeline;
}

static gchar *
format_value (GtkScale * scale, gdouble value)
{
  gint64 real;
  gint64 seconds;
  gint64 subseconds;

  real = value * duration / 100;
  seconds = (gint64) real / GST_SECOND;
  subseconds = (gint64) real / (GST_SECOND / 100);

  return g_strdup_printf ("%02" G_GINT64_FORMAT ":%02" G_GINT64_FORMAT ":%02"
      G_GINT64_FORMAT, seconds / 60, seconds % 60, subseconds % 100);
}

typedef struct
{
  const gchar *name;
  const GstFormat format;
}
seek_format;

static seek_format seek_formats[] = {
  {"tim", GST_FORMAT_TIME},
  {"byt", GST_FORMAT_BYTES},
  {"buf", GST_FORMAT_BUFFERS},
  {"def", GST_FORMAT_DEFAULT},
  {NULL, 0},
};

static void player_query_rates()
{
  GList *walk = rate_pads;

  while (walk) {
    GstPad *pad = GST_PAD (walk->data);
    gint i = 0;

    g_print ("rate/sec  %8.8s: ", GST_PAD_NAME (pad));
    while (seek_formats[i].name) {
      gint64 value;
      GstFormat format;

      format = seek_formats[i].format;

      if (gst_pad_query_convert (pad, GST_FORMAT_TIME, GST_SECOND, &format,
              &value)) {
        g_print ("%s %13" G_GINT64_FORMAT " | ", seek_formats[i].name, value);
      } else {
        g_print ("%s %13.13s | ", seek_formats[i].name, "*NA*");
      }

      i++;
    }
    g_print (" %s:%s\n", GST_DEBUG_PAD_NAME (pad));

    walk = g_list_next (walk);
  }
}

G_GNUC_UNUSED static void
query_positions_elems ()
{
  GList *walk = seekable_elements;

  while (walk) {
    GstElement *element = GST_ELEMENT (walk->data);
    gint i = 0;

    g_print ("positions %8.8s: ", GST_ELEMENT_NAME (element));
    while (seek_formats[i].name) {
      gint64 position, total;
      GstFormat format;

      format = seek_formats[i].format;

      if (gst_element_query_position (element, &format, &position) &&
          gst_element_query_duration (element, &format, &total)) {
        g_print ("%s %13" G_GINT64_FORMAT " / %13" G_GINT64_FORMAT " | ",
            seek_formats[i].name, position, total);
      } else {
        g_print ("%s %13.13s / %13.13s | ", seek_formats[i].name, "*NA*",
            "*NA*");
      }
      i++;
    }
    g_print (" %s\n", GST_ELEMENT_NAME (element));

    walk = g_list_next (walk);
  }
}

G_GNUC_UNUSED static void
query_positions_pads ()
{
  GList *walk = seekable_pads;

  while (walk) {
    GstPad *pad = GST_PAD (walk->data);
    gint i = 0;

    g_print ("positions %8.8s: ", GST_PAD_NAME (pad));
    while (seek_formats[i].name) {
      GstFormat format;
      gint64 position, total;

      format = seek_formats[i].format;

      if (gst_pad_query_position (pad, &format, &position) &&
          gst_pad_query_duration (pad, &format, &total)) {
        g_print ("%s %13" G_GINT64_FORMAT " / %13" G_GINT64_FORMAT " | ",
            seek_formats[i].name, position, total);
      } else {
        g_print ("%s %13.13s / %13.13s | ", seek_formats[i].name, "*NA*",
            "*NA*");
      }

      i++;
    }
    g_print (" %s:%s\n", GST_DEBUG_PAD_NAME (pad));

    walk = g_list_next (walk);
  }
}

static gboolean
update_scale (gpointer data)
{
  GstFormat format;

  position = 0;
  duration = 0;

  format = GST_FORMAT_TIME;

  if (elem_seek) {
    if (seekable_elements) {
      GstElement *element = GST_ELEMENT (seekable_elements->data);

      gst_element_query_position (element, &format, &position);
      gst_element_query_duration (element, &format, &duration);
    }
  } else {
    if (seekable_pads) {
      GstPad *pad = GST_PAD (seekable_pads->data);

      gst_pad_query_position (pad, &format, &position);
      gst_pad_query_duration (pad, &format, &duration);
    }
  }

  if (stats) {
    if (elem_seek) {
      query_positions_elems ();
    } else {
      query_positions_pads ();
    }
    query_rates ();
  }
  if (position >= duration)
    duration = position;

  if (duration > 0) {
    gtk_adjustment_set_value (adjustment, position * 100.0 / duration);
    gtk_widget_queue_draw (hscale);
  }

  return TRUE;
}

static void do_seek (GtkWidget * widget);

static gboolean
end_scrub (GtkWidget * widget)
{
  GST_DEBUG ("end scrub, PAUSE");
  gst_element_set_state (pipeline, GST_STATE_PAUSED);
  seek_timeout_id = 0;

  return FALSE;
}

static gboolean
send_event (GstEvent * event)
{
  gboolean res = FALSE;

  if (!elem_seek) {
    GList *walk = seekable_pads;

    while (walk) {
      GstPad *seekable = GST_PAD (walk->data);

      GST_DEBUG ("send event on pad %s:%s", GST_DEBUG_PAD_NAME (seekable));

      gst_event_ref (event);
      res = gst_pad_send_event (seekable, event);

      walk = g_list_next (walk);
    }
  } else {
    GList *walk = seekable_elements;

    while (walk) {
      GstElement *seekable = GST_ELEMENT (walk->data);

      GST_DEBUG ("send event on element %s", GST_ELEMENT_NAME (seekable));

      gst_event_ref (event);
      res = gst_element_send_event (seekable, event);

      walk = g_list_next (walk);
    }
  }
  gst_event_unref (event);
  return res;
}

static void
do_seek (GtkWidget * widget)
{
  gint64 real;
  gboolean res = FALSE;
  GstEvent *s_event;
  GstSeekFlags flags;

  real = gtk_range_get_value (GTK_RANGE (widget)) * duration / 100;

  flags = 0;
  if (flush_seek)
    flags |= GST_SEEK_FLAG_FLUSH;
  if (accurate_seek)
    flags |= GST_SEEK_FLAG_ACCURATE;
  if (keyframe_seek)
    flags |= GST_SEEK_FLAG_KEY_UNIT;
  if (loop_seek)
    flags |= GST_SEEK_FLAG_SEGMENT;

  s_event = gst_event_new_seek (1.0,
      GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, real, GST_SEEK_TYPE_NONE, 0);

  GST_DEBUG ("seek to %" GST_TIME_FORMAT, GST_TIME_ARGS (real));

  res = send_event (s_event);

  if (res) {
    if (flush_seek) {
      gst_pipeline_set_new_stream_time (GST_PIPELINE (pipeline), 0);
      gst_element_get_state (GST_ELEMENT (pipeline), NULL, NULL, SEEK_TIMEOUT);
    }
  } else
    g_print ("seek failed\n");
}

static void
seek_cb (GtkWidget * widget)
{
  /* If the timer hasn't expired yet, then the pipeline is running */
  if (scrub && seek_timeout_id != 0) {
    GST_DEBUG ("do scrub seek, PAUSED");
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
  }

  GST_DEBUG ("do seek");
  do_seek (widget);

  if (scrub) {
    GST_DEBUG ("do scrub seek, PLAYING");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if (seek_timeout_id == 0) {
      seek_timeout_id =
          g_timeout_add (SCRUB_TIME, (GSourceFunc) end_scrub, widget);
    }
  }
}

static void
set_update_scale (gboolean active)
{
  if (active) {
    if (update_id == 0) {
      update_id =
          g_timeout_add (UPDATE_INTERVAL, (GtkFunction) update_scale, pipeline);
    }
  } else {
    if (update_id) {
      g_source_remove (update_id);
      update_id = 0;
    }
  }
}

static gboolean
start_seek (GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  if (event->type != GDK_BUTTON_PRESS)
    return FALSE;

  if (state == GST_STATE_PLAYING && flush_seek) {
    GST_DEBUG ("start scrub seek, PAUSE");
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
  }

  set_update_scale (FALSE);

  if (changed_id == 0 && flush_seek) {
    changed_id = gtk_signal_connect (GTK_OBJECT (hscale),
        "value_changed", G_CALLBACK (seek_cb), pipeline);
  }

  return FALSE;
}

static gboolean
stop_seek (GtkWidget * widget, gpointer user_data)
{
  if (changed_id) {
    g_signal_handler_disconnect (GTK_OBJECT (hscale), changed_id);
    changed_id = 0;
  }

  if (!flush_seek) {
    GST_DEBUG ("do final seek");
    do_seek (widget);
  }

  if (seek_timeout_id != 0) {
    g_source_remove (seek_timeout_id);
    seek_timeout_id = 0;
    /* Still scrubbing, so the pipeline is already playing */
  } else {
    if (state == GST_STATE_PLAYING) {
      GST_DEBUG ("stop scrub seek, PLAYING");
      gst_element_set_state (pipeline, GST_STATE_PLAYING);
    }
  }
  set_update_scale (TRUE);

  return FALSE;
}

static void
play_cb (GtkButton * button, gpointer data)
{
  GstStateChangeReturn ret;

  if (state != GST_STATE_PLAYING) {
    g_print ("PLAY pipeline\n");
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
      goto failed;

    set_update_scale (TRUE);
    state = GST_STATE_PLAYING;
  }
  return;

failed:
  {
    g_print ("PLAY failed\n");
  }
}

static void
pause_cb (GtkButton * button, gpointer data)
{
  GstStateChangeReturn ret;

  if (state != GST_STATE_PAUSED) {
    g_print ("PAUSE pipeline\n");
    ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE)
      goto failed;

    set_update_scale (FALSE);
    state = GST_STATE_PAUSED;
  }
  return;

failed:
  {
    g_print ("PAUSE failed\n");
  }
}

static void
stop_cb (GtkButton * button, gpointer data)
{
  GstStateChangeReturn ret;

  if (state != GST_STATE_READY) {
    g_print ("READY pipeline\n");
    ret = gst_element_set_state (pipeline, GST_STATE_READY);
    if (ret == GST_STATE_CHANGE_FAILURE)
      goto failed;

    gtk_adjustment_set_value (adjustment, 0.0);
    set_update_scale (FALSE);

    state = GST_STATE_READY;
  }
  return;

failed:
  {
    g_print ("READY failed\n");
  }
}

static void
accurate_toggle_cb (GtkToggleButton * button, GstPipeline * pipeline)
{
  accurate_seek = gtk_toggle_button_get_active (button);
}

static void
key_toggle_cb (GtkToggleButton * button, GstPipeline * pipeline)
{
  keyframe_seek = gtk_toggle_button_get_active (button);
}

static void
loop_toggle_cb (GtkToggleButton * button, GstPipeline * pipeline)
{
  loop_seek = gtk_toggle_button_get_active (button);
}

static void
flush_toggle_cb (GtkToggleButton * button, GstPipeline * pipeline)
{
  flush_seek = gtk_toggle_button_get_active (button);
}

static void
scrub_toggle_cb (GtkToggleButton * button, GstPipeline * pipeline)
{
  scrub = gtk_toggle_button_get_active (button);
}

static void
segment_done (GstBus * bus, GstMessage * message, GstPipeline * pipeline)
{
  GstEvent *event;
  GstSeekFlags flags;
  gboolean res;

  flags = 0;
  if (loop_seek)
    flags |= GST_SEEK_FLAG_SEGMENT;

  event = gst_event_new_seek (1.0,
      GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, 0);

  GST_DEBUG ("segmeent seek to start");

  res = send_event (event);
  if (!res) {
    g_print ("segment seek failed\n");
  }
}

static void
message_received (GstBus * bus, GstMessage * message, GstPipeline * pipeline)
{
  const GstStructure *s;

  s = gst_message_get_structure (message);
  g_print ("message from \"%s\" (%s): ",
      GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (message))),
      gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
  if (s) {
    gchar *sstr;

    sstr = gst_structure_to_string (s);
    g_print ("%s\n", sstr);
    g_free (sstr);
  } else {
    g_print ("no message details\n");
  }
}


typedef struct
{
  gchar *name;
  GstElement *(*func) (const gchar * location);
}
Pipeline;

static Pipeline pipelines[] = {
  {"mp3", make_mp3_pipeline},
  {"avi", make_avi_pipeline},
  {"mpeg1", make_mpeg_pipeline},
  {"mpegparse", make_parse_pipeline},
  {"vorbis", make_vorbis_pipeline},
  {"theora", make_theora_pipeline},
  {"ogg/v/t", make_vorbis_theora_pipeline},
  {"avi/msmpeg4v3/mp3", make_avi_msmpeg4v3_mp3_pipeline},
  {"sid", make_sid_pipeline},
  {"flac", make_flac_pipeline},
  {"wav", make_wav_pipeline},
  {"mod", make_mod_pipeline},
  {"dv", make_dv_pipeline},
  {"mpeg1nothreads", make_mpegnt_pipeline},
  {"playerbin", make_playerbin_pipeline},
  {"parse-launch", make_parselaunch_pipeline},
  {NULL, NULL},
};

#define NUM_TYPES       ((sizeof (pipelines) / sizeof (Pipeline)) - 1)

static void
print_usage (int argc, char **argv)
{
  gint i;

  g_print ("usage: %s <type> <filename>\n", argv[0]);
  g_print ("   possible types:\n");

  for (i = 0; i < NUM_TYPES; i++) {
    g_print ("     %d = %s\n", i, pipelines[i].name);
  }
}

int
main (int argc, char **argv)
{
  GtkWidget *window, *hbox, *vbox, *play_button, *pause_button, *stop_button;
  GtkWidget *accurate_checkbox, *key_checkbox, *loop_checkbox, *flush_checkbox;
  GtkWidget *scrub_checkbox;
  GOptionEntry options[] = {
    {"stats", 's', 0, G_OPTION_ARG_NONE, &stats,
        "Show pad stats", NULL},
    {"elem", 'e', 0, G_OPTION_ARG_NONE, &elem_seek,
        "Seek on elements instead of pads", NULL},
    {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose,
        "Verbose properties", NULL},
    {NULL}
  };
  gint type;
  GOptionContext *ctx;
  GError *err = NULL;

  ctx = g_option_context_new ("seek");
  g_option_context_add_main_entries (ctx, options, NULL);
  g_option_context_add_group (ctx, gst_init_get_option_group ());

  if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
    g_print ("Error initializing: %s\n", err->message);
    exit (1);
  }

  GST_DEBUG_CATEGORY_INIT (seek_debug, "seek", 0, "seek example");

  gtk_init (&argc, &argv);

  if (argc != 3) {
    print_usage (argc, argv);
    exit (-1);
  }

  type = atoi (argv[1]);

  if (type < 0 || type >= NUM_TYPES) {
    print_usage (argc, argv);
    exit (-1);
  }

  pipeline = pipelines[type].func (argv[2]);
  g_assert (pipeline);

  /* initialize gui elements ... */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  hbox = gtk_hbox_new (FALSE, 0);
  vbox = gtk_vbox_new (FALSE, 0);
  play_button = gtk_button_new_with_label ("play");
  pause_button = gtk_button_new_with_label ("pause");
  stop_button = gtk_button_new_with_label ("stop");

  accurate_checkbox = gtk_check_button_new_with_label ("Accurate Seek");
  key_checkbox = gtk_check_button_new_with_label ("Key_unit Seek");
  loop_checkbox = gtk_check_button_new_with_label ("Loop");
  flush_checkbox = gtk_check_button_new_with_label ("Flush");
  scrub_checkbox = gtk_check_button_new_with_label ("Scrub");

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (flush_checkbox), TRUE);

  adjustment =
      GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.00, 100.0, 0.1, 1.0, 1.0));
  hscale = gtk_hscale_new (adjustment);
  gtk_scale_set_digits (GTK_SCALE (hscale), 2);
  gtk_range_set_update_policy (GTK_RANGE (hscale), GTK_UPDATE_CONTINUOUS);

  gtk_signal_connect (GTK_OBJECT (hscale),
      "button_press_event", G_CALLBACK (start_seek), pipeline);
  gtk_signal_connect (GTK_OBJECT (hscale),
      "button_release_event", G_CALLBACK (stop_seek), pipeline);
  gtk_signal_connect (GTK_OBJECT (hscale),
      "format_value", G_CALLBACK (format_value), pipeline);

  /* do the packing stuff ... */
  gtk_window_set_default_size (GTK_WINDOW (window), 250, 96);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_box_pack_start (GTK_BOX (hbox), play_button, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), pause_button, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), stop_button, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), accurate_checkbox, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), key_checkbox, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), loop_checkbox, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), flush_checkbox, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), scrub_checkbox, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hscale, TRUE, TRUE, 2);

  /* connect things ... */
  g_signal_connect (G_OBJECT (play_button), "clicked", G_CALLBACK (play_cb),
      pipeline);
  g_signal_connect (G_OBJECT (pause_button), "clicked", G_CALLBACK (pause_cb),
      pipeline);
  g_signal_connect (G_OBJECT (stop_button), "clicked", G_CALLBACK (stop_cb),
      pipeline);
  g_signal_connect (G_OBJECT (accurate_checkbox), "toggled",
      G_CALLBACK (accurate_toggle_cb), pipeline);
  g_signal_connect (G_OBJECT (key_checkbox), "toggled",
      G_CALLBACK (key_toggle_cb), pipeline);
  g_signal_connect (G_OBJECT (loop_checkbox), "toggled",
      G_CALLBACK (loop_toggle_cb), pipeline);
  g_signal_connect (G_OBJECT (flush_checkbox), "toggled",
      G_CALLBACK (flush_toggle_cb), pipeline);
  g_signal_connect (G_OBJECT (scrub_checkbox), "toggled",
      G_CALLBACK (scrub_toggle_cb), pipeline);

  g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

  /* show the gui. */
  gtk_widget_show_all (window);

  if (verbose) {
    g_signal_connect (pipeline, "deep_notify",
        G_CALLBACK (gst_object_default_deep_notify), NULL);
  }
  {
    GstBus *bus;

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);

//    g_signal_connect (bus, "message::state-changed", (GCallback) message_received, pipeline);
    g_signal_connect (bus, "message::new-clock", (GCallback) message_received,
        pipeline);
    g_signal_connect (bus, "message::error", (GCallback) message_received,
        pipeline);
    g_signal_connect (bus, "message::warning", (GCallback) message_received,
        pipeline);
    g_signal_connect (bus, "message::eos", (GCallback) message_received,
        pipeline);
    g_signal_connect (bus, "message::tag", (GCallback) message_received,
        pipeline);
    g_signal_connect (bus, "message::element", (GCallback) message_received,
        pipeline);
    g_signal_connect (bus, "message::segment-done",
        (GCallback) message_received, pipeline);
    g_signal_connect (bus, "message::segment-done", (GCallback) segment_done,
        pipeline);
  }
  gtk_main ();

  g_print ("NULL pipeline\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_print ("free pipeline\n");
  gst_object_unref (pipeline);

  return 0;
}
