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


#include <libgda/libgda.h>
#include <libgnomedb/libgnomedb.h>


void database_init (int argc, char *argv[]);
void database_list_providers (void);
void database_list_sources (void);
void database_connect (void);

void database_execute_sql (const gchar *sql);
gint database_execute_sql_non_query (const gchar *sql);
GdaDataModel* database_execute_sql_command (const gchar *sql);

void database_show_table (GdaDataModel *dm);

GdaDataModel* database_get_model_from_sql (const gchar * sql);
GdaDataModel* database_get_album_model (void);
GdaDataModel* database_get_artist_model (void);
GdaDataModel* database_get_genre_model (void);
GdaDataModel* database_get_song_model (void);

void database_settings_set_boolean (gchar *group, gchar *key, gboolean value);

