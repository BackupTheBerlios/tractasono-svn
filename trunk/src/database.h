/*
 *      database.h
 *      
 *      Copyright 2007 Patrik Obrist <padx@gmx.net>
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

#ifndef DATABASE_H
#define DATABASE_H


#include "strukturen.h"
#include <sqlite3.h>


// Globale tractasono Datenbank
sqlite3 *db;


// Allgemeines

void db_init (int argc, char *argv[]);
void db_close (void);



// Settings

#define SETTINGS_TYPE_UNDEFINED	0
#define SETTINGS_TYPE_TEXT		1
#define SETTINGS_TYPE_NUMBER	2
#define SETTINGS_TYPE_BOOL		3

void db_settings_set_bool (gchar *group, gchar *key, gboolean value);
gboolean db_settings_get_bool (gchar *group, gchar *key);

void db_settings_set_text (gchar *group, gchar *key, const gchar* value);
gchar* db_settings_get_text (gchar *group, gchar *key);

void db_settings_set_number (gchar *group, gchar *key, gint value);
gint db_settings_get_number (gchar *group, gchar *key);

gint db_track_add (TrackDetails *track);

gint db_artist_add (gchar *artist);

gint db_genre_add (gchar *genre);

gint db_album_add (AlbumDetails *album);

gboolean db_open (void);

gboolean db_check (void);

void db_create (void);


#endif
