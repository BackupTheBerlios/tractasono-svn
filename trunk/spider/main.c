/* Copyright (C) 2003 Scott Wheeler <wheeler@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Includes
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <tag_c.h>
#include <glib.h>

// Defines
#define RETURN_SUCCESS	0
#define RETURN_ERROR	1

// Prototypen
void recursive_dir (const gchar *path);
void print_file_tags (const gchar *path);
gchar *get_export_dir (void);
void create_artist_dir (const gchar *artist);


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
	taglib_set_strings_unicode(FALSE);

	// Pfade bestimmen
	importdir = g_strdup(argv[1]);
	g_message ("import dir: %s", importdir);
	g_message ("export dir: %s", get_export_dir ());

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
			if (strcmp (dirname, ".") == 0 || strcmp (dirname, "..") == 0)
				continue;

			full_path = g_build_filename (path, dirname, NULL);
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
	
	file = taglib_file_new (path);
	
	if (file == NULL) {
		return;
	}
	
	tag = taglib_file_tag (file);

	//g_message ("%s - %s", taglib_tag_artist(tag), taglib_tag_title(tag));
	
	create_artist_dir (taglib_tag_artist(tag));

	/*printf ("-- TAG --\n");
	printf ("title   - \"%s\"\n", taglib_tag_title(tag));
	printf ("artist  - \"%s\"\n", taglib_tag_artist(tag));
	printf ("album   - \"%s\"\n", taglib_tag_album(tag));
	printf ("year    - \"%i\"\n", taglib_tag_year(tag));
	printf ("comment - \"%s\"\n", taglib_tag_comment(tag));
	printf ("track   - \"%i\"\n", taglib_tag_track(tag));
	printf ("genre   - \"%s\"\n", taglib_tag_genre(tag));
	*/

	taglib_tag_free_strings ();
	taglib_file_free (file);
}

gchar *get_export_dir (void)
{
	return g_strdup_printf ("%s/tractasono/music/", g_get_home_dir());
}

void create_artist_dir (const gchar *artist)
{
	gchar *artist_dir;
	
	artist_dir = g_strdup_printf ("%s%s", get_export_dir (), artist);
	
	if (!g_file_test(artist_dir, G_FILE_TEST_EXISTS)) {
		g_mkdir (artist_dir, 493);
		g_message ("Artist dir created: %s", artist_dir);
	}
}
