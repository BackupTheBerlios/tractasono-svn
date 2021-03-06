/*
 *      settings.c
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

#include <string.h>

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <tag_c.h>

#include "settings.h"
#include "interface.h"
#include "keyboard.h"
#include "database.h"
#include "utils.h"


// Prototypen
void settings_import_music (const gchar *path);
void recursive_dir (const gchar *path);
void import_file (const gchar *import_path);
gboolean check_tags (TagLib_Tag *tag);
gchar *get_song_path (TagLib_Tag *tag, gchar *extension);
gboolean register_file (gchar *file);



void settings_init()
{
	g_message ("\tSettings Modul init");
	
	// Aktuelle Version anzeigen
	gchar *text;
	GtkWidget *label_version;
	label_version = interface_get_widget ("label_settings_version");
	if (strcmp (REVISION, "")) {
		text = g_strdup_printf ("%s %s", PACKAGE, REVISION);
	} else {
		text = g_strdup_printf ("%s %s", PACKAGE, VERSION);
	}
	gtk_label_set_text (GTK_LABEL (label_version), text);
	g_free (text);
}


// Handler für Fukuswechler auf dem Settings-Reiter
void on_notebook1_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	gchararray name;
	g_object_get(notebook, "name", &name, NULL);
	//g_print ("Page name: %s\n", name);

	//g_print ("Reiter gewechselt, schliesse Tastatur\n");
	keyboard_show(FALSE);
}


// Fügt eine Zeile Text ein
void settings_import_messages_add (gchar *text)
{
	GtkTextView *view;
	GtkTextBuffer *buf;
	GtkTextIter iter;
	
	view = (GtkTextView*) interface_get_widget ("settings_textview_import");
	buf = gtk_text_view_get_buffer (view);

	gtk_text_buffer_get_end_iter (buf, &iter);
	gtk_text_buffer_insert (buf, &iter, g_strdup_printf ("%s\n", text), -1);
	
}


// Tritt auf, nachdem der "Jetzt importieren" Button gedrückt wurde
void on_button_settings_import_now_clicked (GtkWidget *widget, gpointer user_data)
{
	GtkWidget *entry;
	const gchar *pfad;
	
	entry = interface_get_widget ("settings_entry_import_path");
	
	pfad = gtk_entry_get_text (GTK_ENTRY(entry));
	
	settings_import_music (pfad);
}






void settings_import_music (const gchar *path)
{
	GtkWidget *button;
	gchar *text;
	
	button = interface_get_widget ("button_settings_import_now");
	
	gtk_widget_set_sensitive (button, FALSE);
	
	text = g_strdup_printf ("Starte Import von %s", path);
	settings_import_messages_add (text);
	g_debug ("%s", text);
	
	// Starte Scan
	recursive_dir (path);
	
	text = g_strdup_printf ("Import beendet!");
	settings_import_messages_add (text);
	g_debug ("%s", text);
	
	gtk_widget_set_sensitive (button, TRUE);
}




// Rekursiv durch ein Verzeichnis loopen
void recursive_dir (const gchar *path)
{
	GDir *dir;
	const gchar *dirname, *full_path;

	if ((dir = g_dir_open (path, 0, NULL))) {
		while ((dirname = g_dir_read_name (dir))) {
			/* This should be useless, but we'd better keep it for security */
			g_assert (strcmp (dirname, ".") != 0);
			g_assert (strcmp (dirname, "..") != 0);
			if (strcmp (dirname, ".") == 0 || strcmp (dirname, "..") == 0) {
				continue;
			}

			// Ganzen Pfad zusammenstellen
			full_path = g_build_filename (path, dirname, NULL);
			
			// Export Verzeichnis ebenfalls ignorieren
			if (strcmp (g_strdup_printf("%s/", full_path), get_music_dir ()) == 0) {
				continue;
			}
			
			// Update UI
			while (gtk_events_pending()) {
				gtk_main_iteration();
			}
			
			recursive_dir (full_path);
			g_free ((gchar *) full_path);
		}
		g_dir_close (dir);
	}

	if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
		import_file (path);
	}

	return;
}



// Hier geht die Post ab
void import_file (const gchar *import_path)
{
	TagLib_File *file;
	TagLib_Tag *tag;
	gchar *export_path;
	gchar *filename;
	gchar *artist;
	gchar *album;
	
	// Prüfen ob die Datei gültig ist
	file = taglib_file_new (import_path);
	if (file == NULL) {
		return;
	}
	
	// Prüfen ob wir Tags haben
	tag = taglib_file_tag (file);
	if (tag == NULL) {
		return;
	}
	
	// Falls Tags unvollständig sind -> Nicht importieren
	if (!check_tags (tag)) {
		gchar *text;
		text = g_strdup_printf ("Nicht importieren! (Tags unvollstaendig) -> %s", import_path);
		settings_import_messages_add (text);
		return;
	}
	
	// Dateiname holen
	filename = g_path_get_basename (import_path);
	
	// Die einzelnen Tags holen
	artist = taglib_tag_artist(tag);
	album = taglib_tag_album (tag);
	
	// Artist & Album Ordner erstellen
	create_artist_dir (artist);
	create_album_dir (album, artist);
	
	// Export Pfad bestimmen
	export_path = get_song_path (tag, get_file_extension (filename));
	
	// Prüfen, ob schon vorhanden
	if (exist_target (export_path, import_path)) {
		gchar *text;
		text = g_strdup_printf ("Überspringe: %s", filename);
		settings_import_messages_add (text);
		return;
	}
	
	// Datei kopieren
	if (!copy_file (import_path, export_path)) {
		gchar *text;
		text = g_strdup_printf ("Import fehlgeschlagen! -> %s", filename);
		settings_import_messages_add (text);
		return;
	}
	
	// File in Datenbank eintragen
	if (!register_file (export_path)) {
		gchar *text;
		text = g_strdup_printf ("Datei '%s' konnte nicht in der Datenbank registriert werden!", filename);
		settings_import_messages_add (text);
		return;
	}
	
	// Speicher wieder freigeben
	taglib_tag_free_strings ();
	taglib_file_free (file);
}



// Prüft ob alle Tags gesetzt sind für den Import
gboolean check_tags (TagLib_Tag *tag)
{
	gboolean correct = TRUE;
	gchar *artist, *title, *album;
	
	artist = taglib_tag_artist (tag);
	title = taglib_tag_title (tag);
	album = taglib_tag_album (tag);
	
	if (artist == NULL || strcmp(artist, "") == 0) {
		correct = FALSE;
	}
	if (title == NULL || strcmp(title, "") == 0) {
		correct = FALSE;
	}
	if (album == NULL || strcmp(album, "") == 0) {
		correct = FALSE;
	}
	
	return correct;
}



// Gibt den Dateiname für einen Song zurück
gchar *get_song_path (TagLib_Tag *tag, gchar *extension)
{
	GString *path;
	gchar *artist, *title, *album;
	gint track;
	
	// Tags holen
	artist = g_strdelimit(taglib_tag_artist (tag), "/", '-');
	title = g_strdelimit(taglib_tag_title (tag), "/", '-');
	album = taglib_tag_album (tag);
	track = taglib_tag_track (tag); // Gibt Track-Nr oder 0 zurück
	
	// Starte mit Album Pfad
	path = g_string_new (get_album_dir (album, artist));
	
	
	// Dateiname entsprechend den vorhanden Infos formatieren
	if (track > 0) {
		// Mit Track -> "01 Judas Priest - Painkiller"
		g_string_append_printf (path, "%.2d %s - %s%s", track, artist, title, extension);
	} else {
		// Ohne Track -> "Judas Priest - Painkiller"
		g_string_append_printf (path, "%s - %s%s", artist, title, extension);
	}
	
	// Gesamten Dateinamen zurückgeben
	return path->str;
}


// Datei in Datenbank eintragen
gboolean register_file (gchar *file)
{
	TagLib_File *tagfile;
	TagLib_Tag *tag;
	
	gint id_track;
	gchar *line;
	
	TrackDetails *track;
	
	// Prüfen ob die Datei gültig ist
	tagfile = taglib_file_new (file);
	if (tagfile == NULL) {
		return FALSE;
	}
	
	// Prüfen ob wir Tags haben
	tag = taglib_file_tag (tagfile);
	if (tagfile == NULL) {
		return FALSE;
	}
	
	// Track hinzufügen
	track = track_new ();
	track->number = taglib_tag_track (tag);
	track->path = g_strdup (file);
	track->title = g_strdup (taglib_tag_title (tag));
	track->artist->name = g_strdup (taglib_tag_artist (tag));
	track->album->title = g_strdup (taglib_tag_album (tag));
	track->album->genre = g_strdup (taglib_tag_genre (tag));
	id_track = db_track_add (track);
	
	line = g_strdup_printf ("Importiert: %s - %s (%s)", track->artist->name,
							track->title, track->album->title);
	settings_import_messages_add (line);
	g_free (line);
	
	// Speicher freigeben
	track_free (track);
	taglib_file_free (tagfile);
	
	return TRUE;
}
