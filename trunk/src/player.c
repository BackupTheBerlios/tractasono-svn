/*
 *      player.c
 *      
 *      Copyright 2007 Patrik Obrist <padx@gmx.net>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "player.h"
#include "interface.h"


gboolean KRstopp;

// GStreamer Variablen
GstFormat format;
GstElement *pipeline;
GstElement *src;
GstElement *decoder;
GstElement *audiosink;


// Prototypen
void player_handle_tag_message(GstMessage *message);
static gboolean player_timer_event(GstElement *pipeline);
gboolean player_bus_callback (GstBus *bus, GstMessage *message, gpointer data);



// Player initialisieren
void player_init(int argc, char *argv[])
{
	g_message ("Player init");
	gst_init (&argc, &argv);
	
	// GStreamer Zeitformat einstellen
	format = GST_FORMAT_TIME;
	
	// Pipeline erstellen
	pipeline = gst_element_factory_make("playbin", "tracta-playbin");
	
	// Bus Callback anbinden
	GstBus *bus;
	bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, player_bus_callback, NULL);
	gst_object_unref(bus);
	
	// Watch hinzufügen welcher jede Sekunde auftritt
	g_timeout_add(1000, (GSourceFunc)player_timer_event, pipeline);
}


/*
	Gibt die Länge des Songs in Nanosekunden zurück
*/
gint64 player_get_duration()
{	
	gint64 duration;
	
	if (!gst_element_query_duration(pipeline, &format, &duration)) {
		g_print("Konnte duration nicht herausfinden!\n");
		duration = 0;
	}

	return duration;
}


/*
	Gibt die Position des Songs in Nanosekunden zurück
*/
gint64 player_get_position(void)
{	
	gint64 position;
	
	if (!gst_element_query_position(pipeline, &format, &position)) {
		g_print("Konnte position nicht herausfinden!\n");
	}

	return position;
}

// Callback Behandlung
gboolean player_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	/*g_debug("message from \"%s\" (%s): ",
			GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (message))),
			gst_message_type_get_name (GST_MESSAGE_TYPE (message)));*/
	
	if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_TAG) {
		/* Musik Tags */
		player_handle_tag_message(message);
	}
		
	//g_print ("GStreamer: Got %s message | %d | %d\n", GST_MESSAGE_TYPE_NAME(message), GST_MESSAGE_SRC(message), pipeline);
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
			
			gst_message_parse_state_changed(message, &oldstate, &newstate, &pending);
			g_print("\tStates: (old=%i, new=%i, pending=%i)\n", oldstate, newstate, pending);

			if (newstate == 4) {
				g_print("GStreamer is now playing!\n");
				
				interface_set_playing(TRUE);
				player_set_play ();
				KRstopp = TRUE;
			}
			
			if ((newstate == 3) && (oldstate == 4)) {
				g_print("GStreamer is now paused!\n");
				interface_set_playing(FALSE);
			}
			
		}
		case GST_MESSAGE_EOS: {
			if (g_ascii_strcasecmp(gst_message_type_get_name (GST_MESSAGE_TYPE (message)), "eos") == 0) {
				g_debug("End Of Stream");
				interface_set_playing(FALSE);
			}
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


// Gibt den aktuellen Player Status zurück
gboolean player_get_playing(void)
{
	GstState state;
	
	gst_element_get_state(GST_ELEMENT(pipeline), &state, NULL, GST_CLOCK_TIME_NONE);
	
	if (state != GST_STATE_PLAYING) {
		return FALSE;
	}
	return TRUE;
}

// Play
void player_set_play(void)
{
	gst_element_set_state (GST_ELEMENT(pipeline), GST_STATE_PLAYING);
}

// Pause
void player_set_pause(void)
{
	gst_element_set_state (GST_ELEMENT(pipeline), GST_STATE_PAUSED);
}

// Stop
void player_set_stop(void)
{
	gst_element_set_state (GST_ELEMENT(pipeline), GST_STATE_READY);
}

// Timer Funktion
static gboolean player_timer_event(GstElement *pipeline)
{
	if (player_get_playing()) {
		interface_set_song_position(player_get_position());
	}
	
	/* call me again */
	return TRUE;
}


// Seeking
gboolean player_seek_to_position(gint64 position)
{
	gboolean result;
	
	g_debug("seek to %s", ns_formatted(position));
	
	result = gst_element_seek_simple(GST_ELEMENT(pipeline),
									 format,
									 GST_SEEK_FLAG_FLUSH,
									 position);
	if (!result) {
		g_debug("seek failed!");
	}
	
	return result;
}


// Spiele eine Datei oder einen Stream ab
void player_play_uri(const gchar *uri)
{
	if (pipeline && uri) {
		player_set_stop();
		g_object_set(G_OBJECT(pipeline), "uri", uri, NULL);
		player_set_play();
	} else {
		g_error("pipeline nicht bereit oder uri nicht angegeben!");
	}
}


// Konvertiert Nanosekunden in Sekunden
gint64 ns_to_seconds(gint64 ns)
{	
	gint64 seconds;
	
	seconds = ns / GST_SECOND;
	
	return seconds;
}


// Nanosekunden formatiert in Sekunden und Millisekunden ausgeben
gchar* ns_formatted(gint64 ns)
{	
	gint64 seconds;
	
	seconds = ns_to_seconds(ns);
	
	return g_strdup_printf("%02lli:%02lli", seconds/60, seconds%60);
}


void player_handle_tag_message(GstMessage *message)
{	
	GstTagList *tag_list;
	gchar *artist = NULL;
	gchar *title = NULL;
	gchar *uri = NULL;
	
	gst_message_parse_tag(message, &tag_list);
	
	gst_tag_list_get_string(tag_list, GST_TAG_ARTIST, &artist);
	gst_tag_list_get_string(tag_list, GST_TAG_TITLE, &title);
	gst_tag_list_get_string(tag_list, GST_TAG_LOCATION, &uri);
	
	interface_set_songinfo(artist, title, uri);
	
	g_free(artist);
	g_free(title);
	g_free(uri);
	gst_tag_list_free(tag_list);
}

