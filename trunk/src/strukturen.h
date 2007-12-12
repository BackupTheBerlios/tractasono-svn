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


struct _TrackDetails {
  AlbumDetails *album;
  int number; /* track number */
  char *title;
  char *artist;
  int duration; /* seconds */
  char* track_id;
  char* artist_id;
  GtkTreeIter iter; /* Temporary iterator for internal use */
};

struct _AlbumDetails {
  char* title;
  char* artist;
  char *genre;
  int   number; /* number of tracks in the album */
  GList* tracks;
  GDate *release_date; /* MusicBrainz support multiple releases per album */
  char* album_id;
  char* artist_id;
};

void album_details_free(AlbumDetails *album);
void track_details_free(TrackDetails *track);

#endif
