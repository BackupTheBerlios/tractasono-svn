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
}

// Stop
void player_set_stop()
{
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PAUSED);
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
	
	g_timeout_add (1000, (GSourceFunc) cb_print_position, pipeline);
}

// Seeking
void player_seek_to_position(gint64 position)
{
	g_print("Seeking to position: %lli\n", position);

	GstEvent* seekevent = gst_event_new_seek(1,
							     GST_FORMAT_TIME,
							     GST_SEEK_FLAG_ACCURATE,
							     GST_SEEK_TYPE_SET,
							     position,
							     GST_SEEK_TYPE_NONE,
							     0);

	if (gst_element_send_event(pipeline, seekevent)) {
		g_print("Seek Event konnte verschickt werden!\n");
	}
}
