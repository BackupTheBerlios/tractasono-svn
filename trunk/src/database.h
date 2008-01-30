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

// Gruppen Keys
#define S_G_LIBRARY	"library"


// Keys
#define	S_K_PATH	"path"
#define	S_K_FILECOUNT	"filecount"


#include "strukturen.h"

#include <libgda/libgda.h>
#include <libgnomedb/libgnomedb.h>


void db_init (int argc, char *argv[]);
void db_list_providers (void);
void db_list_sources (void);
void db_connect (void);

GdaDataModel*  db_execute_sql (const gchar *sql);
gint db_execute_sql_non_query (const gchar *sql);

void db_dump_dm (GdaDataModel *dm);

GdaDataModel* db_get_model_from_sql (const gchar * sql);
GdaDataModel* db_get_album_model (void);
GdaDataModel* db_get_artist_model (void);
GdaDataModel* db_get_genre_model (void);
GdaDataModel* db_get_song_model (void);

void db_settings_set_boolean (gchar *group, gchar *key, gboolean value);
gboolean db_settings_get_boolean (gchar *group, gchar *key);

void db_settings_set_string (gchar *group, gchar *key, const gchar* value);
gchar* db_settings_get_string (gchar *group, gchar *key);

void db_settings_set_integer (gchar *group, gchar *key, gint value);
gint db_settings_get_integer (gchar *group, gchar *key);


// Genre Funktionen
gint db_genre_add (gchar *genre);
gint db_genre_get_id (gchar *genre);
const gchar* db_genre_get_name (gint id);
gboolean db_genre_remove (gint id);
gboolean db_genre_rename (gint id, gchar *newname);



// Artist Funktionen
ArtistDetails* db_artist_add (gchar *artist);
gboolean db_artist_update (ArtistDetails* details);

gint db_artist_get_id (gchar *artist);
const gchar* db_artist_get (gint id);

gint db_album_add (gchar *album, gchar *artist);


#endif
