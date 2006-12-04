#include <gtk/gtk.h>
#include <gst/gst.h>
 
#include "player.h"
#include "interface.h"


GstElement *pipeline;
GstElement *src;
GstElement *decoder;
GstElement *audiosink;

static GstState state;

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

/* seek timeout */
#define SEEK_TIMEOUT 40 * GST_MSECOND


GList *seekable_pads = NULL;
GList *rate_pads = NULL;
GList *seekable_elements = NULL;

gint64 position;

gboolean elem_seek = FALSE;
gboolean stats = FALSE;
gboolean accurate_seek = FALSE;
gboolean keyframe_seek = FALSE;
gboolean loop_seek = FALSE;
gboolean flush_seek = TRUE;
gboolean scrub = FALSE;


// Prototypen
GstElement* player_make_flac_pipeline(const gchar * location);
static void player_query_positions_pads();
static void player_query_positions_elems();
static void player_query_rates();
static gboolean player_send_event(GstEvent *event);



// Player initialisieren
void player_init(int *argc, char **argv[])
{
	g_print("Player initialisieren\n");
	gst_init (argc, argv);
}

static GstElement* gst_element_factory_make_or_warn(gchar * type,
													gchar * name)
{
  GstElement *element = gst_element_factory_make(type, name);

  if (!element) {
    g_warning("Failed to create element %s of type %s", name, type);
  }

  return element;
}

/*
	Gibt die Länge des Songs in Sekunden zurück
*/
gint64 player_get_song_duration()
{
	gint64 duration;
	GstFormat fmt = GST_FORMAT_TIME;

	gst_element_query_duration (pipeline, &fmt, &duration);
	
	duration = duration / 1000000000;

	return duration;
}

/*
	Gibt die Länge des Songs in Nanosekunden zurück
*/
gint64 player_get_song_duration_ns()
{
	gint64 duration;
	GstFormat fmt = GST_FORMAT_TIME;

	gst_element_query_duration (pipeline, &fmt, &duration);

	return duration;
}

/*
	Gibt die Position des Songs in Sekunden zurück
*/
gint64 player_get_song_position()
{
	gint64 position;
	GstFormat fmt = GST_FORMAT_TIME;

	gst_element_query_position (pipeline, &fmt, &position);
	
	position = position / 1000000000;

	return position;
}

// Callback Behandlung
static gboolean player_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	//g_print ("GStreamer: Got %s message | %d | %d\n", GST_MESSAGE_TYPE_NAME (message), GST_MESSAGE_SRC(message), pipeline);
	if (GST_MESSAGE_SRC(message) != GST_OBJECT(pipeline)) {
		return TRUE;
	}
	

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("\tError: %s\n", err->message);
			g_error_free (err);
			g_free (debug);
			break;
		}
		case GST_MESSAGE_STATE_CHANGED: {
			GstState oldstate, newstate, pending;
			
			gst_message_parse_state_changed(message,
											&oldstate,
											&newstate,
											&pending);

			/*g_print("\tState Message: old: %i, new: %i, pending: %i\n",
					oldstate,
					newstate,
					pending);*/

			if (newstate == 4) {
				gint64 dur = player_get_song_duration();
				interface_set_playing(TRUE);
				interface_set_song_duration(dur);
				//g_print("Song duration: %lli\n", dur);
				g_print("Player Play\n");
			}
			
			if ((newstate == 3) && (oldstate == 4)) {
				interface_set_playing(FALSE);
				g_print("Player Pause\n");
			}
			
		}
		case GST_MESSAGE_EOS: {
			/* end-of-stream */
			//g_print ("\tEnd Of Stream\n");
			break;
		}
		default: {
			/* unhandled message */
			//g_print ("\tUnhandled Message %i\n", GST_MESSAGE_TYPE (message));
			break;
		}
	}

	/* we want to be notified again the next time there is a message
	* on the bus, so returning TRUE (FALSE means we want to stop watching
	* for messages on the bus and our callback should not be called again) */
	return TRUE;
}

// Play
void player_set_play()
{
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);
	
	state = GST_STATE_PLAYING;
}

// Stop
void player_set_stop()
{
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PAUSED);
	
	state = GST_STATE_PAUSED;
}

static gboolean
cb_print_position (GstElement *pipeline)
{
	if (interface_get_slidermove()) {
		return TRUE;
	}

	gint64 pos;
	
	pos = player_get_song_position();
	
	/*g_print ("Pos: %" GST_TIME_FORMAT "\r", GST_TIME_ARGS (pos));*/
	interface_set_song_position(pos);
	
	/* call me again */
	return TRUE;
}

// Spiele Testfile ab
void player_play_testfile()
{
	pipeline = player_make_flac_pipeline("data/test.flac");
	
	/* adds a watch for new message on our pipeline's message bus to
	* the default GLib main context, which is the main context that our
	* GLib main loop is attached to below */
	GstBus *bus;

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, player_bus_callback, NULL);
	gst_object_unref (bus);
	
	// Watch hinzufügen
	g_timeout_add (1000, (GSourceFunc) cb_print_position, pipeline);
}

// Seeking
void player_seek_to_position(gint seconds)
{
	g_print("Seeking to position: %i\n", seconds);

	GstEvent *event;
	seconds = seconds * GST_SECOND;
	
	if (gst_element_set_state(pipeline, GST_STATE_PAUSED) != GST_STATE_CHANGE_SUCCESS) {
		g_print("Fehler: Konnte state paused nicht setzen!\n");
	}
							 
	event = gst_event_new_seek(1.0,
							   GST_FORMAT_TIME,
							   GST_SEEK_FLAG_FLUSH,
							   GST_SEEK_TYPE_SET,
							   seconds,
							   GST_SEEK_TYPE_NONE,
							   0);

	if (gst_element_send_event (decoder, event)) {
		g_print("Seek Event konnte verschickt werden!\n");
	}
	
	if (gst_element_set_state(pipeline, GST_STATE_PLAYING) != GST_STATE_CHANGE_SUCCESS) {
		g_print("Fehler: Konnte state play nicht setzen!\n");
	}
}

GstElement* player_make_flac_pipeline(const gchar *location)
{
	GstPad *seekable;
	
	pipeline = gst_pipeline_new ("app");
	
	src = gst_element_factory_make_or_warn ("gnomevfssrc", "src");
	decoder = gst_element_factory_make_or_warn ("flacdec", "decoder");
	audiosink = gst_element_factory_make_or_warn ("alsasink", "sink");
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

static gchar* player_format_value(GtkScale * scale, gdouble value)
{
	gint64 real;
	gint64 duration;
	gint64 seconds;
	gint64 subseconds;
	
	duration = player_get_song_duration_ns();
	
	real = value * duration / 100;
	seconds = (gint64) real / GST_SECOND;
	subseconds = (gint64) real / (GST_SECOND / 100);
	
	return g_strdup_printf ("%02" G_GINT64_FORMAT ":%02" G_GINT64_FORMAT ":%02" 
		   G_GINT64_FORMAT, seconds / 60, seconds % 60, subseconds % 100);
}

static gboolean player_update_scale(gpointer data)
{
	GstFormat format;
	
	position = 0;
	
	format = GST_FORMAT_TIME;
	
	if (elem_seek) {
		if (seekable_elements) {
			GstElement *element = GST_ELEMENT (seekable_elements->data);
			
			gst_element_query_position (element, &format, &position);
			//gst_element_query_duration (element, &format, &duration);
		}
	} else {
		if (seekable_pads) {
			GstPad *pad = GST_PAD (seekable_pads->data);
			
			gst_pad_query_position (pad, &format, &position);
			//gst_pad_query_duration (pad, &format, &duration);
		}
	}
	
	if (stats) {
		if (elem_seek) {
			player_query_positions_elems();
		} else {
			player_query_positions_pads();
		}
		player_query_rates();
	}
	
	/*if (position >= duration) {
		duration = position;
	}*/
	
	//if (duration > 0) {
		//gtk_adjustment_set_value (adjustment, position * 100.0 / duration);
		//gtk_widget_queue_draw (hscale);
	//}
	
	return TRUE;
}


static void player_do_seek(GtkWidget *widget)
{
	gint64 real;
	gint64 duration;
	gboolean res = FALSE;
	GstEvent *s_event;
	GstSeekFlags flags;
	
	
	duration = player_get_song_duration_ns();
	
	g_print("Duration: %lli \n", duration);
	
	real = gtk_range_get_value (GTK_RANGE (widget)) * duration / 100;
	
	g_print("Real range: %lli \n", real);
	
	flags = 0;
	if (flush_seek) {
		flags |= GST_SEEK_FLAG_FLUSH;
	}
	if (accurate_seek) {
		flags |= GST_SEEK_FLAG_ACCURATE;
	}
	if (keyframe_seek) {
		flags |= GST_SEEK_FLAG_KEY_UNIT;
	}
	if (loop_seek) {
		flags |= GST_SEEK_FLAG_SEGMENT;
	}
	
	real = 30000000000;
	
	s_event = gst_event_new_seek (1.0, GST_FORMAT_TIME, flags,
								  GST_SEEK_TYPE_SET, real,
								  GST_SEEK_TYPE_NONE, 0);
	
	g_print("seek to %" GST_TIME_FORMAT "\n", GST_TIME_ARGS (real));							  
	//GST_DEBUG ("seek to %" GST_TIME_FORMAT, GST_TIME_ARGS (real));
	
	res = player_send_event (s_event);
	
	if (res) {
		if (flush_seek) {
			gst_pipeline_set_new_stream_time (GST_PIPELINE (pipeline), 0);
			gst_element_get_state (GST_ELEMENT (pipeline), NULL, NULL, SEEK_TIMEOUT);
		}
	} else {
		g_print ("seek failed\n");
	}
}

static void player_query_positions_elems()
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
				g_print ("%s %13.13s / %13.13s | ", seek_formats[i].name, "*NA*", "*NA*");
			}
			i++;
		}
		g_print (" %s\n", GST_ELEMENT_NAME (element));
		
		walk = g_list_next (walk);
	}
}

static void player_query_positions_pads()
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
				g_print ("%s %13.13s / %13.13s | ", seek_formats[i].name, "*NA*", "*NA*");
			}
			i++;
		}
		g_print (" %s:%s\n", GST_DEBUG_PAD_NAME (pad));
		
		walk = g_list_next (walk);
	}
}

static void player_query_rates()
{
	GList *walk = rate_pads;
	
	while (walk) {
		GstPad *pad = GST_PAD(walk->data);
		gint i = 0;
		
		g_print("rate/sec  %8.8s: ", GST_PAD_NAME(pad));
		while (seek_formats[i].name) {
			gint64 value;
			GstFormat format;
			
			format = seek_formats[i].format;
			
			if (gst_pad_query_convert (pad, GST_FORMAT_TIME, GST_SECOND, &format,
				&value)) {
					g_print ("%s %13" G_GINT64_FORMAT " | ", seek_formats[i].name, value);
			} else {
				g_print("%s %13.13s | ", seek_formats[i].name, "*NA*");
			}
			i++;
		}
		g_print(" %s:%s\n", GST_DEBUG_PAD_NAME(pad));
		
		walk = g_list_next(walk);
	}
}

static gboolean player_send_event(GstEvent *event)
{
	gboolean res = FALSE;
	
	/*if (!elem_seek) {
		GList *walk = seekable_pads;
		
		while (walk) {
			GstPad *seekable = GST_PAD(walk->data);
			
			GST_DEBUG("send event on pad %s:%s", GST_DEBUG_PAD_NAME (seekable));
			
			gst_event_ref(event);
			res = gst_pad_send_event(seekable, event);
			
			walk = g_list_next(walk);
		}
	} else {
		GList *walk = seekable_elements;
		
		while (walk) {
			GstElement *seekable = GST_ELEMENT(walk->data);
			
			GST_DEBUG("send event on element %s", GST_ELEMENT_NAME (seekable));
			
			gst_event_ref(event);
			res = gst_element_send_event(seekable, event);
			
			walk = g_list_next(walk);
		}
	}*/
	
	gst_event_ref(event);
	res = gst_element_send_event(pipeline, event);
	gst_event_unref(event);
	
	return res;
}

gboolean player_start_seek(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if (event->type != GDK_BUTTON_PRESS) {
		return FALSE;
	}
	
	if (state == GST_STATE_PLAYING && flush_seek) {
		g_print("start scrub seek, PAUSE\n");
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
	}
	
	//set_update_scale(FALSE);
	
	/*if (flush_seek) {
		gtk_signal_connect(GTK_OBJECT (hscale),
						   "value_changed",
						   G_CALLBACK (seek_cb),
						   pipeline);
	}*/
	
	return FALSE;
}

gboolean player_stop_seek (GtkWidget * widget, gpointer user_data)
{
	/*if (changed_id) {
		g_signal_handler_disconnect (GTK_OBJECT (hscale), changed_id);
		changed_id = 0;
	}*/
	
	g_print("do final seek\n");
	player_do_seek(widget);
	
	if (state == GST_STATE_PLAYING) {
		g_print("stop scrub seek, PLAYING\n");
		gst_element_set_state (pipeline, GST_STATE_PLAYING);
	}
	
	return FALSE;
}

// Formatiert die Scrollbalken Anzeige auf Minuten:Sekunden (00:00)
gchar* on_hscale_song_format_value (GtkScale * scale, gdouble value)
{
	gint64 duration = 0;
	gint64 real;
	gint64 seconds;
	
	if (pipeline) {
		duration = player_get_song_duration_ns();
	}
	
	real = value * duration / 100;
	seconds = (gint64) real / GST_SECOND;

	return g_strdup_printf("%02lli:%02lli", seconds/60, seconds%60);
}
