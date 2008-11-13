/*
 *      utils.c
 *      
 *      Copyright 2007 Patrik Obrist <patrik@gmx.net>
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


#include "utils.h"
#include <gtk/gtk.h>
#include <gio/gio.h>


// Gibt das tractasono Vezeichnis zurück (~/tractasono/)
gchar *get_tractasono_dir (void)
{
	return g_strdup_printf ("%s/tractasono/", g_get_home_dir ());
}

// Gibt das Musik Vezeichnis zurück
gchar *get_music_dir (void)
{
	return g_strdup_printf ("%smusic/", get_tractasono_dir ());
}

// Gibt das Datenbank Vezeichnis zurück
gchar *get_database_dir (void)
{
	return get_tractasono_dir ();
}

// Gibt den Datenbank Dateipfad zurück
gchar *get_database_file (void)
{
	return g_strdup_printf ("%stractasono.db", get_tractasono_dir ());
}

// Gibt den Verzeichnisname für einen Artist zurück
gchar *get_artist_dir (gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_music_dir (), g_strdelimit(artist, "/", '-'));
}

// Erstellt ein Verzeichnis für einen Artist
void create_artist_dir (gchar *artist)
{
	create_dir (get_artist_dir (artist));
}

// Gibt den Verzeichnisname für einen Album zurück
gchar *get_album_dir (gchar *album, gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_artist_dir (artist), g_strdelimit(album, "/", '-'));
}

// Erstellt ein Verzeichnis für einen Album
void create_album_dir (gchar *album, gchar *artist)
{
	create_artist_dir (artist);
	create_dir (get_album_dir (album, artist));
}


// Gibt den Pfad für einen Track zurück
gchar *get_track_path (gchar *track, gchar *album, gchar *artist)
{
	return g_strdup_printf ("%s%s", get_album_dir (album, artist), g_strdelimit(track, "/", '-'));
}

// Gibt den Dateiname eines Tracks ohne Pfad zurück
gchar *get_track_name (gchar *title, gchar *artist, gint tracknr, gchar *extension)
{
	return g_strdup_printf ("%.2d %s - %s%s", tracknr, g_strdelimit(artist, "/", '-'), g_strdelimit(title, "/", '-'), extension);
}

// Vezeichnis mit oder ohne Parent-Vereichnissen erstellen
void create_dir (const gchar *path)
{
	if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
		if (g_mkdir_with_parents (path, 493) != 0) {
			g_error ("Konnte Verzeichnis nicht erstellen -> %s", path);
		}
	}
}

// Gibt die Dateiendung zurück (oder Leerstring falls keine)
gchar *get_file_extension (gchar *path)
{
	gchar *extension;
	
	if (g_str_has_suffix (path, ".flac")) {
		extension = g_strdup_printf (".flac");
	} else if (g_str_has_suffix (path, ".ogg")) {
		extension = g_strdup_printf (".ogg");
	} else if (g_str_has_suffix (path, ".mp3")) {
		extension = g_strdup_printf (".mp3");
	} else {
		extension = "";
	}
	
	return extension;
}

gboolean copy_file (const gchar *source, const gchar *target)
{
	FILE *fp_in;
	FILE *fp_out;
	gchar buffer[BUFFER_SIZE];
	int anz;
	int size;
	
	// Prüfen, ob die Datei bereits vorhanden ist
	if (exist_target (target, source)) {
		return TRUE;
	}

	// Neue Datei zum befüllen anlegen
	fp_out = fopen (target, "w");
	if (fp_out == NULL) {
		return FALSE;
	}
	
	// Datei zum Lesen öffnen
	fp_in = fopen (source, "r");
	if (fp_in == NULL) {
		return FALSE;
	}
	
	// Daten kopieren
	while (!feof(fp_in)) {
		anz = fread (buffer, sizeof(char), sizeof(buffer), fp_in);	
		
		// Update UI
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
		
		size = fwrite (buffer, sizeof(char), anz, fp_out);
		if (size != anz) {
			return FALSE;
		}
	}
	
	// Dateien schliessen
	fclose (fp_in);
	fclose (fp_out);
	
	return TRUE;
}


// Prüfen, ob die Ziel-Datei bereits vorhanden ist
// Vergleicht auch die Dateigrössen
gboolean exist_target (const gchar *target, const gchar *source)
{
	if (g_file_test(target, G_FILE_TEST_EXISTS)) {
		
		GFile *file_source, *file_target;
		GFileInfo *info_source, *info_target;
		goffset size_source, size_target;
		GError *err = NULL;
		
		info_source = g_file_info_new ();
		info_target = g_file_info_new ();

		// Informationen der Quelldatei holen
		file_source = g_file_new_for_path (source); // Hier evtl. URI?
		info_source = g_file_query_info (file_source, G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                         G_FILE_QUERY_INFO_NONE, NULL, &err);
		if (err != NULL) {
			return FALSE;
		}
		
		// Informationen der Zieldatei holen
		file_target = g_file_new_for_path (target); // Hier evtl. URI?
		info_target = g_file_query_info (file_target, G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                         G_FILE_QUERY_INFO_NONE, NULL, &err);
		if (err != NULL) {
			return FALSE;
		}
		
		// Grössenvergleich
		size_source = g_file_info_get_size (info_source);
		size_target = g_file_info_get_size (info_target);
		if (size_source == size_target) {
			return TRUE;
		}
	}
	
	return FALSE;
}


// Prüfen, ob die Ziel-Datei bereits vorhanden ist
gboolean exist_file (const gchar *file)
{
	return g_file_test(file, G_FILE_TEST_EXISTS);
}
