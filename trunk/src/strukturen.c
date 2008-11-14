/*
 *      strukturen.c
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

#include "strukturen.h"

#include <glib.h>


// Neue Artist Struktur erstellen
ArtistDetails* artist_new (void)
{
	ArtistDetails *artist;
	
	artist = g_new0 (ArtistDetails, 1);
	artist->id = 0;
	artist->name = g_strdup ("");
	
	return artist;
}


// Neue Album Struktur erstellen
AlbumDetails* album_new (void)
{
	AlbumDetails *album;
	
	album = g_new0 (AlbumDetails, 1);
	album->id = 0;
	album->title = g_strdup ("");
	album->artist = artist_new ();
	album->genre = g_strdup ("");
	album->number = 0;
	album->compilation = FALSE;
	album->tracks = NULL;
	album->release_date = g_strdup ("");
	album->album_id = g_strdup ("");
	album->artist_id = g_strdup ("");
	
	return album;
}


// Neue Track Struktur erstellen
TrackDetails* track_new (void)
{
	TrackDetails *track;
	
	track = g_new0 (TrackDetails, 1);
	
	track->artist = artist_new ();
	track->album = album_new ();
	track->number = 0;
	track->id = 0;
	track->track_id = g_strdup ("");
	track->artist_id = g_strdup ("");
	track->title = g_strdup ("");
	track->path = g_strdup ("");
	
	return track;
}


/**
 * Free a TrackDetails*
 */
void track_free (TrackDetails *track)
{
	g_return_if_fail (track != NULL);
	g_free (track->title);
	g_free (track->artist);
	g_free (track->path);
	g_free (track->track_id);
	g_free (track->artist_id);
	album_free (track->album);
	artist_free (track->artist);
	g_free (track);
}

/**
 * Free a AlbumDetails*
 */
void album_free (AlbumDetails *album)
{
	g_return_if_fail (album != NULL);
	artist_free (album->artist);
	g_free (album->title);
	g_free (album->artist);
	g_free (album->genre);
	g_free (album->release_date);
	g_free (album->album_id);

	// Alle Tracks freigeben
	GList *t;
	for (t = album->tracks; t != NULL; t = t->next) {
		track_free (t->data);
	}
	g_list_free (album->tracks);
	
	g_free (album);
}


/**
 * Free a ArtistDetails*
 */
void artist_free (ArtistDetails *artist)
{
	g_return_if_fail (artist != NULL);
	g_free (artist->name);
}
