/*
 *      musicbrainz.h
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

#include <musicbrainz3/mb_c.h>
#include <gtk/gtk.h>


typedef struct {
    gchar discid[100];
    gchar disctitle[256];
    MbDisc disc;
} MBDisc;

MBDisc *mb_disc;

void musicbrainz_init (void);

void musicbrainz_read_disc (gchar *drive);
gchar* musicbrainz_get_disctitle (void);
