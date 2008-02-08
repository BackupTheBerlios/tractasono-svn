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
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-utils.h>

// Defines
#define RETURN_SUCCESS	0
#define RETURN_ERROR	1
#define BUFFER_SIZE		9216

// Prototypen
void recursive_dir (const gchar *path);
void export_file (const gchar *import_path);
void create_dir (const gchar *path, gboolean with_parents);
gchar *get_export_dir (void);
gchar *get_artist_dir (gchar *artist);
void create_artist_dir (gchar *artist);
gchar *get_album_dir (gchar *album, gchar *artist);
void create_album_dir (gchar *album, gchar *artist);
gchar *get_song_path (TagLib_Tag *tag, gchar *extension);
gchar *get_file_extension (gchar *path);
gboolean copy_file (const gchar *source, const gchar *target);
gboolean check_tags (TagLib_Tag *tag);
gboolean exist_target (const gchar *source, const gchar *target);


// Hauptprogramm
int main (int argc, char *argv[])
{
	gchar *importdir;
	
	// Prüfe ob ein Pfad übergeben wurde
	if (argc < 2) {
		g_message ("Es wurde kein Import-Verzeichnis angegeben!");
		return RETURN_ERROR;
	}

	// GnomeVFS initialisieren
	gnome_vfs_init ();

	// Braucht es das?
	taglib_set_strings_unicode(TRUE);

	// Pfade bestimmen
	importdir = g_strdup(argv[1]);
	//g_message ("Import dir: %s", importdir);
	//g_message ("Export dir: %s", get_export_dir ());
	
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
		export_file (path);
	}

	return;
}


// Hier geht die Post ab
void export_file (const gchar *import_path)
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
		g_message ("Nicht importieren! (Tags unvollstaendig) -> %s", import_path);
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
	//g_debug("export-Pfad: %s", export_path);
	
	// Datei kopieren
	if (!copy_file (import_path, export_path)) {
		g_warning ("Import fehlgeschlagen! -> %s", filename);
	}
	
	// Speicher wieder freigeben
	taglib_tag_free_strings ();
	taglib_file_free (file);
}


// Vezeichnis mit oder ohne Parent-Vereichnissen erstellen
void create_dir (const gchar *path, gboolean with_parents)
{
	if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
		gint ret;
		if (with_parents) {
			// g_debug("Ordnername: %s", path);
			ret = g_mkdir_with_parents (path, 493);
		} else {
			ret = g_mkdir (path, 493);
		}
		if (ret != 0) {
			g_warning ("Konnte Verzeichnis nicht erstellen -> %s", path);
		}
	}
}

// Gibt Default Export Vezeichnis zurück
gchar *get_export_dir (void)
{
	return g_strdup_printf ("%s/tractasono/music/", g_get_home_dir());
}

// Gibt den Verzeichnisname für einen Artist zurück
gchar *get_artist_dir (gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_export_dir (), g_strdelimit(artist, "/", '-'));
}

// Erstellt ein Verzeichnis für einen Artist
void create_artist_dir (gchar *artist)
{
	create_dir (get_artist_dir (artist), FALSE);
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
	create_dir (get_album_dir (album, artist), FALSE);
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
	
	// Prüfen, ob die Datei bereits vorhanden ist
	if (exist_target (source, target)) {
		//g_message ("Existiert bereits -> %s", source);
		return TRUE;
	} else {
		g_message ("Importiere %s\n", source);
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
		fwrite (buffer, sizeof(char), anz, fp_out);
	}
	
	// Dateien schliessen
	fclose (fp_in);
	fclose (fp_out);
	
	return TRUE;
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


// Prüfen, ob die Ziel-Datei bereits vorhanden ist
// Vegleicht auch die Dateigrössen
gboolean exist_target (const gchar *source, const gchar *target)
{
	if (g_file_test(target, G_FILE_TEST_EXISTS)) {
		
		GnomeVFSFileInfo *info_source, *info_target;
		GnomeVFSResult res;
		GnomeVFSURI *uri_source, *uri_target;
		
		info_source = gnome_vfs_file_info_new ();
		info_target = gnome_vfs_file_info_new ();

		// Dateiinformationen der Quelldatei holen
		uri_source = gnome_vfs_uri_new (gnome_vfs_make_uri_from_input (source));
		if (uri_source == NULL) {
			g_error ("URI %s is NULL", source);
		}
		res = gnome_vfs_get_file_info_uri (uri_source, info_source, GNOME_VFS_FILE_INFO_DEFAULT);
		if (res != GNOME_VFS_OK) {
			g_warning ("'%s' in Quelldatei %s", gnome_vfs_result_to_string (res), source);
			return FALSE;
		}
		
		// Dateiinformationen der Zieldatei holen
		uri_target = gnome_vfs_uri_new (gnome_vfs_get_uri_from_local_path (target));
		if (uri_target == NULL) {
			g_error ("URI %s is NULL", target);
		}
		res = gnome_vfs_get_file_info_uri (uri_target, info_target, GNOME_VFS_FILE_INFO_DEFAULT);
		if (res != GNOME_VFS_OK) {
			g_warning ("'%s' in Zieldatei %s", gnome_vfs_result_to_string (res), target);
			return FALSE;
		}
		
		//g_debug ("Source size: %llu / Target size: %llu", info_source->size, info_target->size);
		
		if (info_source->size == info_target->size) {
			return TRUE;
		}
	}
	
	return FALSE;
}
