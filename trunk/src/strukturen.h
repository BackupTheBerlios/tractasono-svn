/*
 *      strukturen.h
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

#ifndef STRUKTUREN_H
#define STRUKTUREN_H

#include <glib/glist.h>
#include <gtk/gtktreemodel.h>

typedef struct _AlbumDetails AlbumDetails;
typedef struct _TrackDetails TrackDetails;
typedef struct _ArtistDetails ArtistDetails;

struct _TrackDetails {
	ArtistDetails *artist;
	AlbumDetails *album;
	gint id;
	gint number; /* track number */
	gint duration; /* seconds */
	gchar *title;
	gchar *track_id;
	gchar *artist_id;
	gchar *path;
};

struct _AlbumDetails {
	gint id;
	gchar *title;
	ArtistDetails *artist;
	gchar *genre;
	gint   number; /* number of tracks in the album */
	gboolean compilation;
	GList *tracks;
	GDate *release_date; /* MusicBrainz support multiple releases per album */
	gchar *album_id;
	gchar *artist_id;
};

struct _ArtistDetails {
	gint id;
	gchar *name;
};

AlbumDetails* album_new (void);
TrackDetails* track_new (void);
ArtistDetails* artist_new (void);

void album_free (AlbumDetails *album);
void track_free (TrackDetails *track);
void artist_free (ArtistDetails *artist);

#endif
