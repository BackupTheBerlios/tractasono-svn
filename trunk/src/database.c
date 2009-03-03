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
#include "music.h"
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
	if (!db_open ()) {
		g_error ("Die Datenbank konnte nicht geoeffnet werden!");
	}
	
	// UPDATE callback registrieren
	sqlite3_update_hook(db, update_callback, NULL);

	// Testfunktion
	//db_testfunc ();
}


gboolean db_open (void)
{
	gchar *db_file;
	
	db_file = get_database_file ();
	
	if (!exist_file (db_file)) {
		if (!db_create (db_file)) {
			g_unlink (db_file);
			return FALSE;
		}
	}
	
	if (sqlite3_open (db_file, &db) != SQLITE_OK) {
		return FALSE;
	}
	
	// TODO: Auf gültige DB prüfen!
	
	return TRUE;
}


// Das ganze SQL um die tractasono db struktur aufzubauen
gchar *db_get_structure_sql (void)
{
	gboolean ok;
	gchar *filename;
	gchar *content;
	
	filename = get_database_structure_file ();
	if (filename == NULL) {
		return NULL;
	}
	ok = g_file_get_contents (filename, &content, NULL, NULL);                                              
    if (!ok) {
    	return NULL;
	}
	
	return content;
}


// Neue Datenbank erstellen
gboolean db_create (const gchar *db_file)
{
	char *err;
	gchar *sql;
	
	create_dir (get_database_dir ());
	sql = db_get_structure_sql ();
	if (sql == NULL) {
		return FALSE;
	}
	sql = sqlite3_mprintf ("%q", sql);
	if (sqlite3_open (db_file, &db) != SQLITE_OK) {
		return FALSE;
	}
	
	if (sqlite3_exec (db, sql, NULL, NULL, &err) != SQLITE_OK) {
		g_warning ("Es konnte keine neue tractasono Datenbank erstellt werden: %s", err);
		return FALSE;
	}
	
	g_message ("Neue tractasono Datenbank wurde erfolgreich erstellt!");
	return TRUE;
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
	
	
	
	//db_track_add (NULL);
	
	
	
	/*gboolean wert_bool;
	gchar *wert_text;
	gint wert_number;
	
	wert_bool = db_settings_get_bool ("Allgemein", "Firstrun");
	wert_text = db_settings_get_text ("Allgemein", "Version");
	wert_number = db_settings_get_number ("Interface", "AktuellerTab");
	
	g_debug ("Firstrun: %i", wert_bool);
	g_debug ("Version: %s", wert_text);
	g_debug ("Aktueller Tab: %i", wert_number);*/
	
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
	g_message ("Ein UPDATE auf der Tabelle \'%s\' trat ein!", table_name);
	
	
}








gint db_track_id (gchar *track)
{
	gint id = 0;
	char **results;
	gint rows;
	gint cols;
	char *err;
	gchar *sql;
	
	sql = sqlite3_mprintf ("SELECT IDtrack FROM tbl_track WHERE trackname = '%q'", track);
	//g_debug ("SQL: %s", sql);
	if (sqlite3_get_table (db, sql, &results, &rows, &cols, &err)) {
		g_warning ("%s", err);
		return id;
	}
	
	//g_message ("Cols: (%i) / Rows: (%i)", cols, rows);
	if (rows > 0) {
		id = atoi(results[1]);
	}
	
	return id;
}



gint db_artist_id (gchar *artist)
{
	gint id = 0;
	char **results;
	gint rows;
	gint cols;
	char *err;
	gchar *sql;
	
	sql = sqlite3_mprintf ("SELECT IDartist FROM tbl_artist WHERE artistname = '%q'", artist);
	//g_debug ("SQL: %s", sql);
	if (sqlite3_get_table (db, sql, &results, &rows, &cols, &err)) {
		g_warning ("%s", err);
		return id;
	}
	
	if (rows > 0) {
		id = atoi(results[1]);
	}
	
	return id;
}


gint db_genre_id (gchar *genre)
{
	gint id = 0;
	char **results;
	gint rows;
	gint cols;
	char *err;
	gchar *sql;
	
	sql = sqlite3_mprintf ("SELECT IDgenre FROM tbl_genre WHERE genrename = '%q'", genre);
	//g_debug ("SQL: %s", sql);
	if (sqlite3_get_table (db, sql, &results, &rows, &cols, &err)) {
		g_warning ("%s", err);
		return id;
	}
	
	//g_message ("Cols: (%i) / Rows: (%i)", cols, rows);
	if (rows > 0) {
		id = atoi(results[1]);
	}
	
	return id;
}


gint db_album_id (gchar *album)
{
	gint id = 0;
	char **results;
	gint rows;
	gint cols;
	char *err;
	gchar *sql;
	
	sql = sqlite3_mprintf ("SELECT IDalbum FROM tbl_album WHERE albumname = '%q'", album);
	//g_debug ("SQL: %s", sql);
	if (sqlite3_get_table (db, sql, &results, &rows, &cols, &err)) {
		g_warning ("%s", err);
		return id;
	}
	
	//g_message ("Cols: (%i) / Rows: (%i)", cols, rows);
	if (rows > 0) {
		id = atoi(results[1]);
	}
	
	return id;
}


// Füge einen Artist ein, oder mache nichts wenn er schon existiert
gint db_artist_add (ArtistDetails *artist)
{
	gint id = 0;
	char *err;
	gchar *sql;
	
	id = db_artist_id (artist->name);
	if (id != 0) {
		return id;
	}
	
	//sql = g_strdup_printf ("INSERT INTO tbl_artist (artistname) VALUES ('%s')", artist->name);
	sql = sqlite3_mprintf ("INSERT INTO tbl_artist (artistname) VALUES ('%q')", artist->name);
	
	if (sqlite3_exec (db, sql, NULL, NULL, &err) != SQLITE_OK) {
		g_warning ("%s", err);
		return id;
	}
	
	id = db_artist_id (artist->name);
	
	// Sollten hier die Artisten nicht neu geladen werden?
	music_artist_fill ();

	return id;
}



// Füge ein Genre ein, oder mache nichts wenn er schon existiert
gint db_genre_add (gchar *genre)
{
	gint id = 0;
	char *err;
	gchar *sql;
	
	id = db_genre_id (genre);
	if (id != 0) {
		return id;
	}
	
	//sql = g_strdup_printf ("INSERT INTO tbl_genre (genrename) VALUES ('%s')", genre);
	sql = sqlite3_mprintf ("INSERT INTO tbl_genre (genrename) VALUES ('%q')", genre);
	
	if (sqlite3_exec (db, sql, NULL, NULL, &err) != SQLITE_OK) {
		g_warning ("%s", err);
		return id;
	}
	
	id = db_genre_id (genre);
	if (id != 0) {
		return id;
	}
	
	return id;
}


// Füge einen Album ein, oder mache nichts wenn es schon existiert
gint db_album_add (AlbumDetails *album)
{
	gint id = 0;
	char *err;
	gchar *sql;
	
	gint genre = db_genre_add (album->genre);
	
	id = db_album_id (album->title);
	if (id != 0) {
		return id;
	}
	
	sql = sqlite3_mprintf ("INSERT INTO tbl_album (IDgenre, albumname) VALUES (%i, '%q')",  genre, album->title);
	//g_debug ("SQL: %s", sql);
	if (sqlite3_exec (db, sql, NULL, NULL, &err) != SQLITE_OK) {
		g_warning ("%s", err);
		return id;
	}
	
	id = db_album_id (album->title);
	//g_debug ("Album Id: %i", id);
	if (id != 0) {
		return id;
	}
	
	return id;
}





// Füge einen Track ein, oder mache nichts wenn er schon existiert
gint db_track_add (TrackDetails *track)
{
	if (track == NULL) {
		return 0;
	}
	
	gint artist = db_artist_add (track->artist);
	gint album = db_album_add (track->album);
	
	gint id = 0;
	char *err;
	gchar *sql;
	
	id = db_track_id (track->title);
	if (id != 0) {
		return id;
	}
	
	sql = sqlite3_mprintf ("INSERT INTO tbl_track (IDartist, IDalbum, trackname, trackpath, tracknumber) VALUES (%i, %i, '%q', '%q', %i)", artist, album, track->title, track->path, track->number);
	//g_debug ("SQL: %s", sql);
	if (sqlite3_exec (db, sql, NULL, NULL, &err) != SQLITE_OK) {
		g_warning ("%s", err);
		return id;
	}
	
	id = db_track_id (track->title);
	if (id != 0) {
		return id;
	}
	
	return id;
	
}
