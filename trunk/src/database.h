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


void db_init (int argc, char *argv[]);
void db_close (void);


void db_execute_sql (const gchar *sql, gint (*callback)(void*,gint,gchar**,gchar**));

gint db_get_table (	const char *sql,       /* SQL to be executed */
					char ***resultp,       /* Result written to a char *[]  that this points to */
					int *nrow,             /* Number of result rows written here */
					int *ncolumn,          /* Number of result columns written here */
					char **errmsg          /* Error msg written here */);


gint db_execute_sql_non_query (const gchar *sql);

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
