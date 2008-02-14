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

#include <glib/glist.h>


/**
 * Free a TrackDetails*
 */
void track_details_free(TrackDetails *track)
{
	g_return_if_fail (track != NULL);
	g_free (track->title);
	g_free (track->artist);
	g_free (track->track_id);
	g_free (track->artist_id);
	g_free (track);
}

/**
 * Free a AlbumDetails*
 */
void album_details_free(AlbumDetails *album)
{
	g_return_if_fail (album != NULL);
	g_free (album->title);
	g_free (album->artist);
	g_free (album->genre);
	g_free (album->album_id);
	if (album->release_date) {
		g_date_free (album->release_date);
	}
	//g_list_deep_free (album->tracks, (GFunc)track_details_free);
	g_list_free (album->tracks);
	g_free (album);
}


/**
 * Free a ArtistDetails*
 */
void artist_details_free(ArtistDetails *artist)
{
	g_free (artist->name);
}