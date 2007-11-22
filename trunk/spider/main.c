/*
 *      main.c
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


// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <tag_c.h>
#include <glib.h>
#include <glib/gstdio.h>

// Defines
#define RETURN_SUCCESS	0
#define RETURN_ERROR	1

// Prototypen
void recursive_dir (const gchar *path);
void print_file_tags (const gchar *path);
void create_dir (const gchar *path, gboolean parents);
gchar *get_export_dir (void);
gchar *get_artist_dir (const gchar *artist);
void create_artist_dir (const gchar *artist);
gchar *get_album_dir (const gchar *album, const gchar *artist);
void create_album_dir (const gchar *album, const gchar *artist);
gchar *get_song_path (const gchar *album, const gchar *artist, const gchar *title, gint track, gchar *filename);
gchar *get_file_extension (gchar *path);


// Hauptprogramm
int main (int argc, char *argv[])
{
	gchar *importdir;
	
	// Prüfe ob ein Pfad übergeben wurde
	if (argc < 2) {
		g_message ("Es wurde kein Import-Verzeichnis angegeben!");
		return RETURN_ERROR;
	}

	// Braucht es das?
	taglib_set_strings_unicode(TRUE);

	// Pfade bestimmen
	importdir = g_strdup(argv[1]);
	g_message ("import dir: %s", importdir);
	g_message ("export dir: %s", get_export_dir ());
	
	// Export Verzeichnis anlegen
	create_dir (get_export_dir (), TRUE);

	// Starte Scan
	recursive_dir (importdir);

	return RETURN_SUCCESS;
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
			if (strcmp (g_strdup_printf("%s/", full_path), get_export_dir ()) == 0) {
				continue;
			}
			
			recursive_dir (full_path);
			g_free ((gchar *) full_path);
		}
		g_dir_close (dir);
	}

	if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
		//g_debug ("File: %s", path);
		print_file_tags (path);
	}

	return;
}


// Tag Informationen ausgeben
void print_file_tags (const gchar *path)
{
	TagLib_File *file;
	TagLib_Tag *tag;
	gchar *filename;
	gchar *artist;
	gchar *title;
	gchar *album;
	gint track;
	
	// Prüfen ob die Datei gültig ist
	file = taglib_file_new (path);
	if (file == NULL) {
		return;
	}
	
	// Prüfen ob wir Tags haben
	tag = taglib_file_tag (file);
	if (tag == NULL) {
		return;
	}
	
	// Dateiname holen
	filename = g_path_get_basename (path);
	
	// Die einzelnen Tags holen
	artist = taglib_tag_artist(tag);
	title = taglib_tag_title(tag);
	album = taglib_tag_album (tag);
	track = taglib_tag_track (tag);

	//g_message("Song: %s", get_song_path (album, artist, title, track, filename));
	
	// Artist & Album Ordner erstellen
	create_artist_dir (artist);
	create_album_dir (album, artist);
	
	gchar *systemcall;
	systemcall = g_strdup_printf ("cp \"%s\" \"%s\"", path, get_song_path (album, artist, title, track, filename));
	if (!system (systemcall)) {
		g_message ("Kopieren erfolgreich!");
	} else {
		g_message ("Kopieren fehlgeschlagen!");
	}
	
	// Speicher wieder freigeben
	taglib_tag_free_strings ();
	taglib_file_free (file);
}


// Vezeichnis mit oder ohne Parent-Vereichnissen erstellen
void create_dir (const gchar *path, gboolean parents)
{
	if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
		gint ret;
		if (parents) {
			ret = g_mkdir_with_parents (path, 493);
		} else {
			ret = g_mkdir (path, 493);
		}
		if (ret == 0) {
			g_message ("Directory created: %s", path);	
		} else {
			g_message ("Cannot create directory: %s", path);
		}
	}
}

// Gibt Default Export Vezeichnis zurück
gchar *get_export_dir (void)
{
	return g_strdup_printf ("%s/tractasono/music/", g_get_home_dir());
}

// Gibt den Verzeichnisname für einen Artist zurück
gchar *get_artist_dir (const gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_export_dir (), artist);
}

// Erstellt ein Verzeichnis für einen Artist
void create_artist_dir (const gchar *artist)
{
	create_dir (get_artist_dir (artist), FALSE);
}

// Gibt den Verzeichnisname für einen Album zurück
gchar *get_album_dir (const gchar *album, const gchar *artist)
{
	return g_strdup_printf ("%s%s/%s/", get_export_dir (), artist, album);
}

// Erstellt ein Verzeichnis für einen Album
void create_album_dir (const gchar *album, const gchar *artist)
{
	create_artist_dir (artist);
	create_dir (get_album_dir (album, artist), FALSE);
}

// Gibt den Dateiname für einen Song zurück
gchar *get_song_path (const gchar *album, const gchar *artist, const gchar *title, gint track, gchar *filename)
{
	GString *path;
	gchar *extension;
	
	// Bestimme Dateiendung
	extension = get_file_extension (filename);
	
	// Dateiname entsprechend den vorhanden Infos formatieren
	if ( (artist == NULL || strcmp(artist, "") == 0) || (title == NULL || strcmp(title, "") == 0) || (album == NULL || strcmp(album, "") == 0) ) {
		// Nur Original Dateiname verwenden
		path = g_string_new (get_export_dir ());
		g_string_append_printf (path, "Unknown/Unknown/%s", filename);
	} else {
		// Tags verwenden
		path = g_string_new (get_album_dir (album, artist));
		if (track > 0) {
			// Mit Track -> "01 Judas Priest - Painkiller"
			g_string_append_printf (path, "%.2d %s - %s%s", track, artist, title, extension);
		} else {
			// Ohne Track -> "Judas Priest - Painkiller"
			g_string_append_printf (path, "%s - %s%s", artist, title, extension);
		}
	}
	
	return path->str;
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
		extension = g_strdup_printf ("");
	}
	
	return extension;
}
