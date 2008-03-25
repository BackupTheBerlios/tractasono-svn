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


// Prototypen
void db_testfunc (void);
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
	g_message ("Datenbank Tesfunktion");
	
	gboolean wert_bool;
	gchar *wert_text;
	gint wert_number;
	
	wert_bool = db_settings_get_bool ("Allgemein", "Firstrun");
	wert_text = db_settings_get_text ("Allgemein", "Version");
	wert_number = db_settings_get_number ("Interface", "AktuellerTab");
	
	g_debug ("Firstrun: %i", wert_bool);
	g_debug ("Version: %s", wert_text);
	g_debug ("Aktueller Tab: %i", wert_number);
	
}


void db_settings_set_bool (gchar *group, gchar *key, gboolean value)
{
	g_message ("Settings: set boolean");
}


gboolean db_settings_get_bool (gchar *group, gchar *key)
{
	gboolean val;
	gint rc;
	gchar *sql;
	sqlite3_stmt *stmt;
	const char *tail;
	
	val = FALSE;
	sql = g_strdup_printf ("SELECT settingsvalue FROM tbl_settings WHERE settingsgroup = '%s' AND settingskey = '%s' AND settingstype = %i", group, key, SETTINGS_TYPE_BOOL);
	
	rc = sqlite3_prepare (db, sql, strlen (sql), &stmt, &tail);
	if (rc != SQLITE_OK) {
		g_error ("SQL error: %s", sqlite3_errmsg (db));
	}
	
	rc = sqlite3_step (stmt);
	if (rc == SQLITE_ROW) {
		if (sqlite3_column_int (stmt, 0)) {
			val = TRUE;
		}
	}
	
	sqlite3_finalize(stmt);
	
	return val;
}


void db_settings_set_text (gchar *group, gchar *key, const gchar* value)
{
	g_message ("Settings: set string");
}


gchar* db_settings_get_text (gchar *group, gchar *key)
{
	gchar *text;
	gint rc;
	gchar *sql;
	sqlite3_stmt *stmt;
	const char *tail;
	
	text = NULL;
	sql = g_strdup_printf ("SELECT settingsvalue FROM tbl_settings WHERE settingsgroup = '%s' AND settingskey = '%s' AND settingstype = %i", group, key, SETTINGS_TYPE_TEXT);
	
	rc = sqlite3_prepare (db, sql, strlen (sql), &stmt, &tail);
	if (rc != SQLITE_OK) {
		g_error ("SQL error: %s", sqlite3_errmsg (db));
	}
	
	rc = sqlite3_step (stmt);
	if (rc == SQLITE_ROW) {
		text = (gchar*) sqlite3_column_text (stmt, 0);
	}
	
	sqlite3_finalize(stmt);
	
	return text;
}


void db_settings_set_number (gchar *group, gchar *key, gint value)
{
	/*gint number;
	gint rc;
	gchar *sql;
	sqlite3_stmt *stmt;
	const char *tail;
	
	number = 0;
	sql = g_strdup_printf ("UPDATE tbl_settings SET settingsvalue = 23 WHERE settingsgroup = 'Interface' AND settingskey = 'AktuellerTab'", group, key, SETTINGS_TYPE_NUMBER);
	
	rc = sqlite3_prepare (db, sql, strlen (sql), &stmt, &tail);
	if (rc != SQLITE_OK) {
		g_error ("SQL error: %s", sqlite3_errmsg (db));
	}
	
	rc = sqlite3_step (stmt);
	if (rc == SQLITE_ROW) {
		number = sqlite3_column_int (stmt, 0);
	}
	
	sqlite3_finalize(stmt);*/
}


gint db_settings_get_number (gchar *group, gchar *key)
{
	gint number;
	gint rc;
	gchar *sql;
	sqlite3_stmt *stmt;
	const char *tail;
	
	number = 0;
	sql = g_strdup_printf ("SELECT settingsvalue FROM tbl_settings WHERE settingsgroup = '%s' AND settingskey = '%s' AND settingstype = %i", group, key, SETTINGS_TYPE_NUMBER);
	
	rc = sqlite3_prepare (db, sql, strlen (sql), &stmt, &tail);
	if (rc != SQLITE_OK) {
		g_error ("SQL error: %s", sqlite3_errmsg (db));
	}
	
	rc = sqlite3_step (stmt);
	if (rc == SQLITE_ROW) {
		number = sqlite3_column_int (stmt, 0);
	}
	
	sqlite3_finalize(stmt);
	
	return number;
}


void update_callback (void * data,
					  int operation_code,
					  char const *db_name,
					  char const *table_name,
					  sqlite_int64 rowid)
{
	g_message ("Ein UPDATE auf der Tabelle \'%s\'trat ein!", table_name);
	
	
}


