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


#define DATABASE "tractasono"


GdaConnection *conn;


void db_init (int argc, char *argv[])
{
	g_message ("Datenbank init");
	
	gda_init (PACKAGE, VERSION, argc, argv);
	//gnome_db_init (PACKAGE, VERSION, argc, argv);
	
	//db_list_providers ();
	//db_list_sources ();
	db_connect ();
	
	// Test Funktionen
	//db_settings_set_string ("library", "path", "/opt/music/");
	//db_settings_set_string ("library", "name", "Die geilschti Musig!");
	//db_settings_set_integer ("library", "filecount", 122);
	//g_debug ("Pfad: %s", db_settings_get_string ("library", "path"));
	//g_debug ("Name: %s", db_settings_get_string ("library", "name"));
	//g_debug ("Filecount: %i", db_settings_get_integer (S_G_LIBRARY, S_K_FILECOUNT));
	
	ArtistDetails *artist;
	
	artist = g_new0 (ArtistDetails, 1);
	artist->name = g_strdup ("Sonata Arctica");
	artist->id = db_artist_add (artist->name);
	
	g_message ("artist: id=%d, name=%s", artist->id, artist->name);
	
}


void db_list_providers (void)
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


void db_list_sources (void)
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


void db_connect (void)
{
	GError *error = NULL;
	GdaDataSourceInfo *info = NULL;
	GdaClient *client;
      
	client = gda_client_new ();
 
	info = gda_config_find_data_source (DATABASE);
	if (!info)
		g_error ("DSN '%s' is not declared", DATABASE);
	else {
		conn = gda_client_open_connection (client, info->name, 
												 info->username, info->password,
												 0, &error);							 								 
		if (!conn) {
			g_warning ("Can't open connection to DSN %s: %s\n", info->name,
					   error && error->message ? error->message : "???");
			return;
		}
		g_message ("\tsuccessfully connected to: %s", info->name);
		gda_data_source_info_free (info);
		g_object_unref (G_OBJECT (client));
	}
	
}


GdaDataModel* db_get_model_from_sql (const gchar * sql)
{
	GError *error = NULL;
	GdaDataModel *model;
	GdaCommand *command;
	
	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	model = gda_connection_execute_select_command (conn, command, NULL, &error);
	gda_command_free (command);
	if (error) {
		g_print ("Error: %s", error->message);
	}
	return model;
}



GdaDataModel* db_get_album_model (void)
{
	return db_get_model_from_sql("SELECT * FROM tbl_album");
}

GdaDataModel* db_get_artist_model (void)
{
	return db_get_model_from_sql("SELECT * FROM tbl_artist");
}

GdaDataModel* db_get_genre_model (void)
{
	return db_get_model_from_sql("SELECT * FROM tbl_genre");
}

GdaDataModel* db_get_song_model (void)
{
	return db_get_model_from_sql("SELECT * FROM view_song_short");
}



GdaDataModel* db_execute_sql (const gchar *sql)
{
	GdaCommand *command;
	GError *error = NULL;
	GdaDataModel *dm;

	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	dm = gda_connection_execute_select_command (conn, command, NULL, &error);
	if (error) {
		g_warning (error->message);
	}
	gda_command_free (command);
	
	//db_dump_dm (dm);
	
	return dm;
}


gint db_execute_sql_non_query (const gchar *sql)
{
	GdaCommand *command;
	GError *error = NULL;
	gint count;

	command = gda_command_new (sql, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	count  = gda_connection_execute_non_select_command (conn, command, NULL, &error);
	if (error) {
		g_message (error->message);
	}

	gda_command_free (command);

	return count;
}


void db_dump_dm (GdaDataModel *dm)
{	
	g_print ("\n%s\n", gda_data_model_dump_as_string (dm));
}


void db_settings_set_boolean (gchar *group, gchar *key, gboolean value)
{
	GString *sql;
	GdaDataModel *dm;
	gint id = 0;
	GValue *val;
	
	// Schaue ob Eintrag schon existiert
	sql = g_string_new ("SELECT IDsettings FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = db_execute_sql (sql->str);
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
	
	db_execute_sql (sql->str);
}


gboolean db_settings_get_boolean (gchar *group, gchar *key)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gboolean value = FALSE;
	
	sql = g_string_new ("SELECT settingsboolean FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		if (strcmp(gda_value_stringify (val), "1") == 0) {
			value = TRUE;
		}
	}
	
	return value;
}


void db_settings_set_string (gchar *group, gchar *key, const gchar* value)
{
	GString *sql;
	GdaDataModel *dm;
	gint id = 0;
	GValue *val;
	
	// Schaue ob Eintrag schon existiert
	sql = g_string_new ("SELECT IDsettings FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = db_execute_sql (sql->str);
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
	
	db_execute_sql (sql->str);
}


gchar* db_settings_get_string (gchar *group, gchar *key)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gchar* value = NULL;
	
	sql = g_string_new ("SELECT settingsstring FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		value = gda_value_stringify (val);
	}
	
	return value;
}


void db_settings_set_integer (gchar *group, gchar *key, gint value)
{
	GString *sql;
	GdaDataModel *dm;
	gint id = 0;
	GValue *val;
	
	// Schaue ob Eintrag schon existiert
	sql = g_string_new ("SELECT IDsettings FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = db_execute_sql (sql->str);
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
	
	db_execute_sql (sql->str);
}


gint db_settings_get_integer (gchar *group, gchar *key)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gint value = 0;
	
	sql = g_string_new ("SELECT settingsinteger FROM tbl_settings");
	g_string_append_printf (sql, " WHERE settingsgroup = '%s' AND settingskey = '%s'", group, key);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		value = g_value_get_int (val);
	}
	
	return value;
}



gint db_artist_add (gchar *name)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	gint id;
	
	// Schauen, ob Record schon existiert
	id = db_artist_get_id (name);
	if (id) {
		// Datensatz Record zurückgeben
		return id;
	}
		
	// Datensatz neu erstellen
	sql = g_string_new ("INSERT INTO tbl_artist (artistname)");
	g_string_append_printf (sql, " VALUES ('%s')", name);
	db_execute_sql (sql->str);
	
	return db_artist_get_id (name);
}


gint db_artist_get_id (gchar *name)
{
	GString *sql;
	GdaDataModel *dm;
	GValue *val;
	
	sql = g_string_new ("SELECT IDartist FROM tbl_artist");
	g_string_append_printf (sql, " WHERE artistname = '%s'", name);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	return g_value_get_int (val);
}
