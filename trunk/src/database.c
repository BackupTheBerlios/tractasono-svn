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

#include <string.h>

#include "database.h"
#include "settings.h"


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
	
	// Test Funktionen
	//database_settings_set_string ("library", "path", "/opt/music/");
	//database_settings_set_string ("library", "name", "Die geilschti Musig!");
	//database_settings_set_integer ("library", "filecount", 122);
	//g_debug ("Pfad: %s", database_settings_get_string ("library", "path"));
	//g_debug ("Name: %s", database_settings_get_string ("library", "name"));
	//g_debug ("Filecount: %i", database_settings_get_integer ("library", "filecount"));
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



GdaDataModel* database_execute_sql_command (const gchar *sql)
{
	GdaCommand *command;
	GError *error = NULL;
	GdaDataModel *dm;

	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	dm = gda_connection_execute_select_command (connection, command, NULL, &error);
	if (error) {
		g_message (error->message);
	}
	gda_command_free (command);
	
	//database_dump_dm (dm);
	
	return dm;
}


void database_execute_sql (const gchar *sql)
{
	GdaCommand *command;
	GError *error = NULL;

	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL,
				   GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	gda_connection_execute_select_command (connection, command, NULL, &error);
	if (error) {
		g_message (error->message);
	}

	gda_command_free (command);
}


gint database_execute_sql_non_query (const gchar *sql)
{
	GdaCommand *command;
	GError *error = NULL;
	gint count;

	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	count  = gda_connection_execute_non_select_command (connection, command, NULL, &error);
	if (error) {
		g_message (error->message);
	}

	gda_command_free (command);

	return count;
}


void database_dump_dm (GdaDataModel *dm)
{	
	g_print ("\n%s\n", gda_data_model_dump_as_string (dm));
}


void database_settings_set_boolean (gchar *group, gchar *key, gboolean value)
{
	GString *sql;
	GdaDataModel *dm;
	gint id = 0;
	GValue *val;
	
	// Schaue ob Eintrag schon existiert
	sql = g_string_new ("SELECT IDsettings FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = database_execute_sql_command (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		// Datensatz mutieren
		id = g_value_get_int (val);
		sql = g_string_new ("UPDATE tbl_settings SET");
		g_string_append_printf (sql, " settingsboolean = '%i' WHERE IDsettings = '%i'", value, id);
	} else {
		// Datensatz neu erstellen
		sql = g_string_new ("INSERT INTO tbl_settings(settingsgroup, settingskey, settingsboolean)");
		g_string_append_printf (sql, " VALUES ('%s', '%s', '%i')", group, key, value);
	}
	
	database_execute_sql (sql->str);
}


gboolean database_settings_get_boolean (gchar *group, gchar *key)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gboolean value = FALSE;
	
	sql = g_string_new ("SELECT settingsboolean FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = database_execute_sql_command (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		if (strcmp(gda_value_stringify (val), "1") == 0) {
			value = TRUE;
		}
	}
	
	return value;
}


void database_settings_set_string (gchar *group, gchar *key, const gchar* value)
{
	GString *sql;
	GdaDataModel *dm;
	gint id = 0;
	GValue *val;
	
	// Schaue ob Eintrag schon existiert
	sql = g_string_new ("SELECT IDsettings FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = database_execute_sql_command (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		// Datensatz mutieren
		id = g_value_get_int (val);
		sql = g_string_new ("UPDATE tbl_settings SET");
		g_string_append_printf (sql, " settingsstring = '%s' WHERE IDsettings = '%i'", value, id);
	} else {
		// Datensatz neu erstellen
		sql = g_string_new ("INSERT INTO tbl_settings(settingsgroup, settingskey, settingsstring)");
		g_string_append_printf (sql, " VALUES ('%s', '%s', '%s')", group, key, value);
	}
	
	database_execute_sql (sql->str);
}


gchar* database_settings_get_string (gchar *group, gchar *key)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gchar* value = NULL;
	
	sql = g_string_new ("SELECT settingsstring FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = database_execute_sql_command (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		value = gda_value_stringify (val);
	}
	
	return value;
}


void database_settings_set_integer (gchar *group, gchar *key, gint value)
{
	GString *sql;
	GdaDataModel *dm;
	gint id = 0;
	GValue *val;
	
	// Schaue ob Eintrag schon existiert
	sql = g_string_new ("SELECT IDsettings FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = database_execute_sql_command (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		// Datensatz mutieren
		id = g_value_get_int (val);
		sql = g_string_new ("UPDATE tbl_settings SET");
		g_string_append_printf (sql, " settingsinteger = '%i' WHERE IDsettings = '%i'", value, id);
	} else {
		// Datensatz neu erstellen
		sql = g_string_new ("INSERT INTO tbl_settings(settingsgroup, settingskey, settingsinteger)");
		g_string_append_printf (sql, " VALUES ('%s', '%s', '%i')", group, key, value);
	}
	
	database_execute_sql (sql->str);
}


gint database_settings_get_integer (gchar *group, gchar *key)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gint value = 0;
	
	sql = g_string_new ("SELECT settingsinteger FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = database_execute_sql_command (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		value = g_value_get_int (val);
	}
	
	return value;
}

