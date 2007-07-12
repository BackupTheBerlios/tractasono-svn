/*
 *      musicbrainz.c
 *      
 *      Copyright 2007 Patrik Obrist <patrik@valhalla>
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

#include <stdio.h>
#include <string.h>
#include <musicbrainz3/mb_c.h>

#include "musicbrainz.h"


void musicbrainz_init (void)
{
	g_message ("\tMusicbrainz init");
	
	mb_disc = g_slice_new (MBDisc);	
}



void musicbrainz_read_disc (gchar *drive)
{
	int i, size;
	MbQuery q;
    MbReleaseFilter f;
    MbResultList results;

    mb_disc->disc = mb_read_disc (drive);
    if (!mb_disc->disc) {
        g_warning ("Konnte Disc nicht lesen!");
        return;
    }

    mb_disc_get_id (mb_disc->disc, mb_disc->discid, 100);
    g_print ("Disc Id: %s\n\n", mb_disc->discid);

    q = mb_query_new (NULL, NULL);
    f = mb_release_filter_new ();
    mb_release_filter_disc_id (f, mb_disc->discid);
    results = mb_query_get_releases (q, f);
    mb_release_filter_free (f);
    mb_query_free (q);
    if (!results) {
        g_warning ("Konnte keine Resultate empfangen!");
        return;
    }

    size = mb_result_list_get_size (results);
    for (i = 0; i < size; i++) {
        MbRelease release = mb_result_list_get_release (results, i);
        mb_release_get_id (release, mb_disc->discid, 100);
        g_print ("Id    : %s\n", mb_disc->discid);
        mb_release_get_title (release, mb_disc->disctitle, 256);
        g_print ("Title : %s\n\n", mb_disc->disctitle);
        mb_release_free (release);
    }
    mb_result_list_free (results);

    return;
}


gchar* musicbrainz_get_disctitle (void)
{
	if (!mb_disc) {
		return "Kein Disc Titel verfügbar!";
	}
	
	if (strcmp (mb_disc->disctitle, "") == 0) {
		return mb_disc->discid;
	} else {
		return mb_disc->disctitle;
	}
}
