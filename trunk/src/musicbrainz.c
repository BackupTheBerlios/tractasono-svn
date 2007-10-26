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



MbDisc *disc;


void musicbrainz_init (void)
{
	g_message ("\tMusicbrainz init");
}



void musicbrainz_read_disc (gchar *drive)
{
	/*int i, size;
	MbQuery q;
    MbReleaseFilter f;
    MbResultList results;
    GString *id;
    
    id = g_string_new_len ("", 100);

    disc = mb_read_disc (drive);
    
    if (!disc) {
    	g_warning ("Konnte Disc nicht einlesen!");
    	return;
    }
    
    mb_disc_get_id (disc, id->str, 100);
    g_message ("Disc Id: %s", id->str);

    q = mb_query_new (NULL, NULL);
    f = mb_release_filter_new ();
    mb_release_filter_disc_id (f, m_disc.discid);
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
        mb_release_get_id (release, m_disc.discid, 100);
        g_print ("Id    : %s\n", m_disc.discid);
        mb_release_get_title (release, m_disc.disctitle, 256);
        g_print ("Title : %s\n\n", m_disc.disctitle);
        mb_release_free (release);
    }
    mb_result_list_free (results);

    return;*/
}


gchar* musicbrainz_get_disctitle (void)
{	
	/*if (strcmp (m_disc.disctitle, "") == 0) {
		return m_disc.discid;
	} else {
		return m_disc.disctitle;
	}*/
	
	return "";
}

