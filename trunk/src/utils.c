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


// Gibt Default Musik Vezeichnis zurück
gchar *get_music_dir (void)
{
	return g_strdup_printf ("%s/tractasono/music/", g_get_home_dir());
}

// Gibt den Verzeichnisname für einen Artist zurück
gchar *get_artist_dir (const gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_music_dir (), artist);
}

// Erstellt ein Verzeichnis für einen Artist
void create_artist_dir (const gchar *artist)
{
	create_dir (get_artist_dir (artist), FALSE);
}

// Gibt den Verzeichnisname für einen Album zurück
gchar *get_album_dir (const gchar *album, const gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_artist_dir (artist), album);
}

// Erstellt ein Verzeichnis für einen Album
void create_album_dir (const gchar *album, const gchar *artist)
{
	create_artist_dir (artist);
	create_dir (get_album_dir (album, artist), FALSE);
}


// Gibt den Pfad für einen Track zurück
gchar *get_track_path (const gchar *track, const gchar *album, const gchar *artist)
{
	return g_strdup_printf ("%s%s", get_album_dir (album, artist), track);
}

// Gibt den Dateiname eines Tracks ohne Pfad zurück
gchar *get_track_name (const gchar *title, const gchar *artist, gint tracknr, gchar *extension)
{
	return g_strdup_printf ("%.2d %s - %s%s", tracknr, artist, title, extension);
}

// Vezeichnis mit oder ohne Parent-Vereichnissen erstellen
void create_dir (const gchar *path, gboolean with_parents)
{
	if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
		gint ret;
		if (with_parents) {
			ret = g_mkdir_with_parents (path, 493);
		} else {
			ret = g_mkdir (path, 493);
		}
		if (ret != 0) {
			g_warning ("Konnte Verzeichnis nicht erstellen -> %s", path);
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
	
	// Prüfen, ob die Datei bereits vorhanden ist
	if (exist_target (source, target)) {
		g_message ("Existiert bereits -> %s", source);
		return TRUE;
	} else {
		g_message ("Wird importiert -> %s", source);
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


// Prüfen, ob die Ziel-Datei bereits vorhanden ist
gboolean exist_target (const gchar *source, const gchar *target)
{
	if (g_file_test(target, G_FILE_TEST_EXISTS)) {
		
		GnomeVFSFileInfo *info_source, *info_target;
		
		info_source = gnome_vfs_file_info_new ();
		info_target = gnome_vfs_file_info_new ();
		
		if (gnome_vfs_get_file_info (source, info_source, 0) != GNOME_VFS_OK) {
			return FALSE;
		}
		
		if (gnome_vfs_get_file_info (target, info_target, 0) != GNOME_VFS_OK) {
			return FALSE;
		}
		
		//g_debug ("Source size: %llu / Target size: %llu", info_source->size, info_target->size);
		
		if (info_source->size == info_target->size) {
			return TRUE;
		}
	}
	
	return FALSE;
}