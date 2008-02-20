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
#include "utils.h"
#include <string.h>


// Globale tractasono Datenbank
sqlite3 *db;


// Prototypen
void db_testfunc (void);
static gint callback (void *NotUsed, gint argc, gchar **argv, gchar **azColName);
void update_callback (void * data, int operation_code,
					  char const *db_name, char const *table_name,
					  sqlite_int64 rowid);


void db_init (int argc, char *argv[])
{
	g_message ("Database init");
	
	// Mit Datenbank verbinden
	if (sqlite3_open (get_database_file (), &db)) {
		g_error ("Can't open database: %s", sqlite3_errmsg (db));
	}
	
	// UPDATE callback registrieren
	sqlite3_update_hook(db, update_callback, NULL);

	
	// Testfunktion
	//db_testfunc ();
}


void db_close (void)
{
	g_message ("Database close");
	
	// Datenbank wieder schliessen
	sqlite3_close (db);
}


void db_testfunc (void)
{
	gint rc;
	gchar *msg = NULL;

	rc = sqlite3_exec (db, "SELECT COUNT(*) AS AnzahlArtists FROM tbl_artist", callback, 0, &msg);
	if( rc != SQLITE_OK ){
		g_warning ("SQL error: %s", msg);
		sqlite3_free (msg);
	}
	
	
	// Test Funktionen
	//db_settings_set_string ("library", "path", "/opt/music/");
	//db_settings_set_string ("library", "name", "Die geilschti Musig!");
	//db_settings_set_integer ("library", "filecount", 122);
	//g_debug ("Pfad: %s", db_settings_get_string ("library", "path"));
	//g_debug ("Name: %s", db_settings_get_string ("library", "name"));
	//g_debug ("Filecount: %i", db_settings_get_integer (S_G_LIBRARY, S_K_FILECOUNT));
	
	//ArtistDetails *artist;
	
	//artist = g_new0 (ArtistDetails, 1);
	//artist->name = g_strdup ("Sonata Arctica");
	//artist->id = db_artist_add (artist->name);
	
	//g_message ("artist: id=%d, name=%s", artist->id, artist->name);
	
	
	//g_message ("genre: name=%s", db_genre_get (db_genre_get_id ("Rock")));
	//g_message ("artist: name=%s", db_artist_get (db_artist_get_id ("Hammerfall")));
	
	//ArtistDetails *test;
	//test = db_artist_add ("Bratney Spears");
	//g_message ("Artist: %s, Id: %d", test->name, test->id);
	//test->name = g_strdup ("Brät Wurst");
	//db_artist_update (test);
	//g_free (test);
	
}



void db_execute_sql (const gchar *sql, gint (*callback)(void*,gint,gchar**,gchar**))
{
	gint rc;
	gchar *msg = NULL;

	rc = sqlite3_exec (db, sql, callback, 0, &msg);
	if( rc != SQLITE_OK ){
		g_warning ("SQL error: %s", msg);
		sqlite3_free (msg);
	}
}


gint db_get_table (	const char *sql,       /* SQL to be executed */
					char ***resultp,       /* Result written to a char *[]  that this points to */
					int *nrow,             /* Number of result rows written here */
					int *ncolumn,          /* Number of result columns written here */
					char **errmsg          /* Error msg written here */)
{
	return sqlite3_get_table (db, sql, resultp, nrow, ncolumn, errmsg);
}



static gint callback (void *NotUsed, gint rows, gchar **cols, gchar **titles)
{
	gint row;
	
	for(row = 0; row < rows; row++){
		g_print ("%s = %s\n", titles[row], cols[row] ? cols[row] : "NULL");
	}
	
	g_print ("\n");
	return 0;
}





gint db_execute_sql_non_query (const gchar *sql)
{


	return 0;
}




void db_settings_set_boolean (gchar *group, gchar *key, gboolean value)
{
	/*GString *sql;
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
	
	db_execute_sql (sql->str);*/
}


gboolean db_settings_get_boolean (gchar *group, gchar *key)
{
	/*GString *sql;
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
	
	return value;*/
	
	return TRUE;
}


void db_settings_set_string (gchar *group, gchar *key, const gchar* value)
{
	/*GString *sql;
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
	
	db_execute_sql (sql->str);*/
}


gchar* db_settings_get_string (gchar *group, gchar *key)
{
	/*GString *sql;
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
	
	return value;*/
	
	return NULL;
}


void db_settings_set_integer (gchar *group, gchar *key, gint value)
{
	/*GString *sql;
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
	
	db_execute_sql (sql->str);*/
}


gint db_settings_get_integer (gchar *group, gchar *key)
{
	/*GString *sql;
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
	
	return value;*/
	
	return 0;
}



gint db_genre_add (gchar *genre)
{
	/*GString *sql;
	gint id;
	
	// Schauen, ob Record schon existiert
	id = db_genre_get_id (genre);
	if (id) {
		// Datensatz Record zurückgeben
		return id;
	}
		
	// Datensatz neu erstellen
	sql = g_string_new ("INSERT INTO tbl_genre (genrename)");
	g_string_append_printf (sql, " VALUES ('%s')", genre);
	db_execute_sql (sql->str);
	
	return db_artist_get_id (genre);*/
	
	return 0;
}



gint db_genre_get_id (gchar *genre)
{
	/*GString *sql;
	GdaDataModel *dm;
	GValue *val;
	
	sql = g_string_new ("SELECT IDgenre FROM tbl_genre");
	g_string_append_printf (sql, " WHERE genrename = '%s'", genre);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	return g_value_get_int (val);*/
	
	return 0;
}


const gchar* db_genre_get (gint id)
{
	/*GString *sql;
	GdaDataModel *dm;
	GValue *val;
	
	sql = g_string_new ("SELECT genrename FROM tbl_genre");
	g_string_append_printf (sql, " WHERE IDgenre = '%d'", id);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	return g_value_get_string (val);*/
	
	return NULL;
}




ArtistDetails* db_artist_add (gchar *artist)
{
	/*GString *sql;
	gint id;
	ArtistDetails* details;
	
	details = g_new0 (ArtistDetails, 1);
	details->name = g_strdup (artist);
	
	// Schauen, ob Record schon existiert
	id = db_artist_get_id (artist);
	if (id) {
		// Datensatz Record zurückgeben
		details->id = id;
		
		return details;
	}
		
	// Datensatz neu erstellen
	sql = g_string_new ("INSERT INTO tbl_artist (artistname)");
	g_string_append_printf (sql, " VALUES ('%s')", artist);
	db_execute_sql (sql->str);
	
	details->id = db_artist_get_id (artist);
	
	return details;*/
	
	return NULL;
}



gboolean db_artist_update (ArtistDetails* details)
{
	/*GString *sql;
	
	sql = g_string_new ("UPDATE tbl_artist SET ");
	g_string_append_printf (sql, "artistname = '%s'", details->name);
	g_string_append_printf (sql, " WHERE IDartist = %d", details->id);
	
	//g_message ("SQL: %s", sql->str);
	
	return db_execute_sql_non_query (sql->str);*/
	
	return TRUE;
}






gint db_artist_get_id (gchar *artist)
{
	/*GString *sql;
	GdaDataModel *dm;
	GValue *val;
	
	sql = g_string_new ("SELECT IDartist FROM tbl_artist");
	g_string_append_printf (sql, " WHERE artistname = '%s'", artist);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	if (val) {
		return g_value_get_int (val);
	} else {
		return FALSE;	
	}*/
	
	return TRUE;
}



const gchar* db_artist_get (gint id)
{
	/*GString *sql;
	GdaDataModel *dm;
	GValue *val;
	
	sql = g_string_new ("SELECT artistname FROM tbl_artist");
	g_string_append_printf (sql, " WHERE IDartist = '%d'", id);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	return g_value_get_string (val);*/
	
	return NULL;
}




gint db_album_add (gchar *album, gchar *artist)
{
	/*GString *sql;
	gint id;
	
	// Schauen, ob Record schon existiert
	id = db_artist_get_id (artist);
	if (id) {
		// Datensatz Record zurückgeben
		return id;
	}
		
	// Datensatz neu erstellen
	sql = g_string_new ("INSERT INTO tbl_artist (artistname)");
	g_string_append_printf (sql, " VALUES ('%s')", artist);
	db_execute_sql (sql->str);
	
	return db_artist_get_id (artist);*/
	
	return 0;
}


gint db_album_get_id (gchar *album, gchar *artist)
{
	/*GString *sql;
	GdaDataModel *dm;
	GValue *val;
	
	sql = g_string_new ("SELECT IDartist FROM tbl_artist");
	g_string_append_printf (sql, " WHERE artistname = '%s'", artist);
	dm = db_execute_sql (sql->str);
	val = (GValue*) gda_data_model_get_value_at (dm, 0, 0);
	
	return g_value_get_int (val);*/
	
	return 0;
}




void update_callback (void * data, int operation_code,
					  char const *db_name, char const *table_name,
					  sqlite_int64 rowid)
{
	g_message ("Ein UPDATE trat ein!");
	
	
}


