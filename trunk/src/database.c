/*
 *      database.c
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "database.h"


#define DATABASE "tractasono"

GdaConnection *connection;


void database_init (int argc, char *argv[])
{
	g_message ("Datenbank init");
	
	gda_init (PACKAGE, VERSION, argc, argv);
	gnome_db_init (PACKAGE, VERSION, argc, argv);
	
	//database_list_providers ();
	//database_list_sources ();
	database_connect ();
}


void database_list_providers (void)
{
    GList *prov_list;
    GList *node;
    GdaProviderInfo *info;

    prov_list = gda_config_get_provider_list ();

    for (node = g_list_first (prov_list); node != NULL; node = node->next) {
        info = (GdaProviderInfo *) node->data;
        g_debug ("database providers: ID=%s Location=%s\n", info->id, info->location);
    }
    gda_config_free_provider_list (prov_list);
}


void database_list_sources (void)
{
    GList *ds_list;
    GList *node;
    GdaDataSourceInfo *info;

    ds_list = gda_config_get_data_source_list ();
    for (node = g_list_first (ds_list); node != NULL; node = g_list_next (node)) {
        info = (GdaDataSourceInfo *) node->data;
        g_debug ("NAME: %s PROVIDER: %s CNC: %s DESC: %s USER: %s PASSWORD: %s",
                 info->name, info->provider, info->cnc_string, info->description,
                 info->username, info->password);
    }
    gda_config_free_data_source_list (ds_list);
}


void database_connect (void)
{
	GError *error = NULL;
	GdaDataSourceInfo *info = NULL;
	GdaClient *client;
      
	client = gda_client_new ();
 
	info = gda_config_find_data_source (DATABASE);
	if (!info)
		g_error ("DSN '%s' is not declared", DATABASE);
	else {
		connection = gda_client_open_connection (client, info->name, 
												 info->username, info->password,
												 0, &error);							 								 
		if (!connection) {
			g_warning ("Can't open connection to DSN %s: %s\n", info->name,
			   error && error->message ? error->message : "???");
			   return;
		}
		gda_data_source_info_free (info);
		g_object_unref (G_OBJECT (client));
	}
	
}


gint database_execute_sql_non_query (const gchar * buffer)
{
	GError *error = NULL;
	GdaCommand *command;
	gint number;
	
	command = gda_command_new (buffer, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	number = gda_connection_execute_non_select_command (connection, command, NULL, &error);
	if (error) {
		g_print ("Error: %s", error->message);
	}
	gda_command_free (command);
	
	return number;
}


GdaDataModel* database_get_model_from_sql (const gchar * sql)
{
	GError *error = NULL;
	GdaDataModel *model;
	GdaCommand *command;
	
	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	model = gda_connection_execute_select_command (connection, command, NULL, &error);
	gda_command_free (command);
	if (error) {
		g_print ("Error: %s", error->message);
	}
	return model;
}



GdaDataModel* database_get_album_model (void)
{
	return database_get_model_from_sql("SELECT * FROM tbl_album");
}

GdaDataModel* database_get_artist_model (void)
{
	return database_get_model_from_sql("SELECT * FROM tbl_artist");
}

GdaDataModel* database_get_genre_model (void)
{
	return database_get_model_from_sql("SELECT * FROM tbl_genre");
}

GdaDataModel* database_get_song_model (void)
{
	return database_get_model_from_sql("SELECT * FROM view_song_short");
}



