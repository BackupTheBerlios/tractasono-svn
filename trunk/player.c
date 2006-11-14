#include <gtk/gtk.h>
#include <gst/gst.h>
 
#include "player.h"
#include "interface.h"


GstElement *pipeline;

// Player initialisieren
void player_init(int *argc, char **argv[])
{
	g_print("Player initialisieren\n");
	gst_init (argc, argv);
}

// Callback Behandlung
static gboolean player_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	//g_print ("GStreamer: Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

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
			//GstState *oldstate, *newstate, *pending;
			/*gst_message_parse_state_changed (message,
											 oldstate,
                                             newstate,
                                             pending);*/
			//g_print("\told: %d, new: %d, pending: %d", oldstate, newstate, pending);
		}
		case GST_MESSAGE_EOS: {
			/* end-of-stream */
			g_print ("\tEnd Of Stream\n");
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
	
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 len;

	gst_element_query_duration (pipeline, &fmt, &len);
	g_print ("Range lenght: %d\n", (gdouble)len);
	
	interface_set_songinfo("hallo", "velo", len);
}

// Stop
void player_set_stop()
{
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PAUSED);
}

static gboolean
cb_print_position (GstElement *pipeline)
{
  GstFormat fmt = GST_FORMAT_TIME;
  gint64 pos, len;

  if (gst_element_query_position (pipeline, &fmt, &pos)
    && gst_element_query_duration (pipeline, &fmt, &len)) {
    g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
	     GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
	     //g_print ("pos %lli\r", pos/GST_SECOND); 
	interface_set_position_in_song(pos/1000000000);
	}

  /* call me again */
  return TRUE;
}

// Spiele Testfile ab
void player_play_testfile()
{
	GstElement *source, *filter, *sink;
	GstBus *bus;

	pipeline = gst_pipeline_new ("my-pipeline");
	
	source = gst_element_factory_make ("filesrc", "source");
	g_object_set (source, "location", "data/test.mp3", NULL);

	filter = gst_element_factory_make ("mad", "filter");
	sink = gst_element_factory_make ("alsasink", "sink");

	gst_bin_add_many (GST_BIN (pipeline), source, filter, sink, NULL);

	/* adds a watch for new message on our pipeline's message bus to
	* the default GLib main context, which is the main context that our
	* GLib main loop is attached to below */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, player_bus_callback, NULL);
	gst_object_unref (bus);

	if (!gst_element_link_many (source, filter, sink, NULL)) {
		g_warning ("Failed to link elements!");
	}
	
	g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
}
