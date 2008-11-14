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
#include "music.h"


// GStreamer Variablen
GstFormat format;
GstElement *pipeline;
GstElement *src;
GstElement *decoder;
GstElement *audiosink;

// Playliste
PlayList *the_list;

// Prototypen
void player_handle_tag_message (GstMessage *message);
static gboolean player_timer_event (GstElement *pipeline);
gboolean player_bus_callback (GstBus *bus, GstMessage *message, gpointer data);
const gchar* get_next_uri ();
const gchar* get_prev_uri ();


gint player_test (void)
{
	const gchar uri[] = "http://85.214.59.226:9000";
	//const gchar uri[] = "file:///opt/music/DragonForce/Inhuman Rampage/DragonForce - Cry for Eternity.ogg";
	//const gchar uri[] = "http://www.xiph.org/vorbis/listen/41_30secOgg-b64M.ogg";
	//const gchar uri[] = "http://ogg2.smgradio.com/vr32.ogg";
	//const gchar uri[] = "http://network.virginradio.co.uk/core/audio/ogg/live.pls?service=vr";
	
	gst_element_set_state (pipeline, GST_STATE_NULL);

	g_object_set (G_OBJECT (pipeline), "uri", uri, NULL);

	
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	return 0;
}


// Player initialisieren
void player_init (int argc, char *argv[])
{	
	g_message ("Player init");
	
	// GStreamer initialisieren
	g_message ("\tGStreamer init");
	gst_init (&argc, &argv);
	
	// GStreamer Zeitformat einstellen
	format = GST_FORMAT_TIME;
	
	// Pipeline erstellen
	pipeline = gst_element_factory_make ("playbin", "player");
	
	// Wir wollen die GNOME Einstellungen für die Ausgabe verwenden!
	// Ausgabegerät kann unter System -> Einstellungen -> Audio
	// ausgewählt werden (Profil "Musik und Filme")
	GstElement *sink;
	sink = gst_element_factory_make ("gconfaudiosink", "sink");
	if (sink) {
    	g_object_set (G_OBJECT (sink), "profile", 1, NULL);
    	g_object_set (G_OBJECT (pipeline), "audio-sink", sink, NULL);
    }
	
	// Bus Callback anbinden
	GstBus *bus;
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, player_bus_callback, NULL);
	gst_object_unref (bus);

	//gst_xml_write_file (GST_ELEMENT (pipeline), fopen ("xmlTest.gst", "w"));
	
	// Watch hinzufügen welcher jede Sekunde auftritt
	g_timeout_add (1000, (GSourceFunc) player_timer_event, pipeline);
	
	// Playlist mit NULL initialisieren
	the_list = NULL;
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
	//g_message ("GStreamer -> Got \"%s\" message from \"%s\"", GST_MESSAGE_TYPE_NAME(message), GST_ELEMENT_NAME (GST_MESSAGE_SRC (message)));
	
	if (GST_MESSAGE_TYPE (message) == GST_MESSAGE_TAG) {
		/* Musik Tags */
		//g_message ("GStreamer -> Got \"%s\" message from \"%s\"", GST_MESSAGE_TYPE_NAME(message), GST_ELEMENT_NAME (GST_MESSAGE_SRC (message)));
		player_handle_tag_message (message);
	}
	
	if (GST_MESSAGE_SRC(message) != GST_OBJECT(pipeline)) {		
		return TRUE;
	}

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gst_message_parse_error (message, &err, NULL);
			g_warning ("%s", err->message);
			g_error_free (err);
			break;
		}
		case GST_MESSAGE_STATE_CHANGED: {
			GstState oldstate, newstate, pending;
			
			gst_message_parse_state_changed(message, &oldstate, &newstate, &pending);
			//g_message ("States: (old=%i, new=%i, pending=%i)", oldstate, newstate, pending);

			if (newstate == 4) {
				g_message ("GStreamer is now playing!\n");
				interface_set_playing (player_get_state ());
			}
			if ((newstate == 2) && (oldstate == 3)) {
				g_message ("GStreamer is now ready(stoped)!\n");
				interface_set_playing (STATE_PLAY_NOTHING);
			}
			if ((newstate == 3) && (oldstate == 4)) {
				g_message ("GStreamer is now paused!\n");
				interface_set_playing (STATE_PLAY_NOTHING);
			}
		}
		case GST_MESSAGE_EOS: {
			if (g_ascii_strcasecmp(gst_message_type_get_name (GST_MESSAGE_TYPE (message)), "eos") == 0) {
				g_message ("End Of Stream");
				if (!player_play_next ()) {
					interface_set_playing (STATE_PLAY_NOTHING);
				}
			}
			break;
		}
		default: {
			/* unhandled message */
			//g_debug ("Unhandled Message %i", GST_MESSAGE_TYPE (message));
			break;
		}
	}

	/* we want to be notified again the next time there is a message
	* on the bus, so returning TRUE (FALSE means we want to stop watching
	* for messages on the bus and our callback should not be called again) */
	return TRUE;
}


// Gibt den zurück ob der Player etwas Spielt
gboolean player_get_playing(void)
{
	GstState state;
	
	gst_element_get_state(GST_ELEMENT(pipeline), &state, NULL, GST_MSECOND);
	
	if (state != GST_STATE_PLAYING) {
		return FALSE;
	}
	return TRUE;
}


// Gibt den aktuellen Player Status zurück (STATE_PLAY_NOTHING, STATE_PLAY_LOCAL, STATE_PLAY_STREAM)
PlayerState player_get_state(void)
{
	GstState state;
	
	gst_element_get_state(GST_ELEMENT(pipeline), &state, NULL, GST_MSECOND);
	
	if (state != GST_STATE_PLAYING) {
		return STATE_PLAY_NOTHING;
	} else {
		if(player_get_duration () > 0){ // Wenn duration grösser 0 ist Musik == lokal
			return STATE_PLAY_LOCAL;
		} else {
			return STATE_PLAY_STREAM;		
		}          
	}
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
static gboolean player_timer_event (GstElement *pipeline)
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
void player_play_uri (const gchar *uri)
{
	g_return_if_fail (pipeline != NULL);
	
	interface_update_controls (the_list);
	
	if (uri) {
		player_set_stop();
		g_object_set(G_OBJECT (pipeline), "uri", uri, NULL);
		player_set_play();
		g_debug ("Player spielt jetzt URI: %s", uri);
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
	
	return g_strdup_printf ("%li:%li", (long int)seconds/60, (long int)seconds%60);
}


void player_handle_tag_message(GstMessage *message)
{	
	GstTagList *tag_list;
	gchar *artist = NULL;
	gchar *title = NULL;
	gchar *uri = NULL;
	gchar *discid = NULL;
	
	gst_message_parse_tag (message, &tag_list);
	
	gst_tag_list_get_string (tag_list, GST_TAG_ARTIST, &artist);
	gst_tag_list_get_string (tag_list, GST_TAG_TITLE, &title);
	gst_tag_list_get_string (tag_list, GST_TAG_LOCATION, &uri);
	gst_tag_list_get_string (tag_list, GST_TAG_CDDA_MUSICBRAINZ_DISCID, &discid);

	//g_message ("DiscID: %s", discid);
	
	interface_set_songinfo (artist, title);
	
	g_free (artist);
	g_free (title);
	g_free (uri);
	gst_tag_list_free (tag_list);
}


gboolean player_play_next ()
{
	g_return_val_if_fail (the_list != NULL, FALSE);
	
	gchar *uri;
	playlist_next (the_list);
	uri = playlist_get_uri (the_list);
	player_play_uri (uri);
	
	return TRUE;
}


gboolean player_play_prev ()
{
	g_return_val_if_fail (the_list != NULL, FALSE);

	gchar *uri;
	playlist_prev (the_list);
	uri = playlist_get_uri (the_list);
	player_play_uri (uri);
	
	return TRUE;
}


void dump_playlist (GList *playlist)
{
	int i = 1;
	GList *temp = g_list_first (playlist);
	
	while (temp) {
		g_debug ("Playlist Element %i: %s", i, (gchar*)temp->data);
		temp = g_list_next (temp);
		i++;
	}
	
		
	
}


void player_play_playlist (PlayList *playlist, gint pos)
{
	g_return_if_fail (playlist != NULL);
	
	gchar *uri;
	
	// Vorhandene Playlist gegebenenfalls freigeben
	if (the_list) {
		playlist_free (the_list);
	}
	
	the_list = playlist;
	playlist_nth (playlist, pos);
	uri = playlist_get_uri (the_list);
	if (!uri) {
		g_warning ("PlayList ist leer!");
	}
	
	player_play_uri (uri);
}



