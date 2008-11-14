/*
 *      playlist.h
 *      
 *      Copyright 2008 Patrik Obrist <patrik@gmx.net>
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

#ifndef PLAYLIST_H
#define PLAYLIST_H


typedef struct _PlayList PlayList;


struct _PlayList {
	GList *list;
};


PlayList* playlist_new (void);

void playlist_add_uri (PlayList *playlist, const gchar *uri);
gchar* playlist_get_uri (PlayList *playlist);

gboolean playlist_next (PlayList *playlist);
gboolean playlist_prev (PlayList *playlist);

gboolean playlist_nth (PlayList *playlist, gint pos);

void playlist_free (PlayList *playlist);


#endif
