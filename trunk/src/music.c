/*
 *      music.c
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

#include <string.h>
#include <tag_c.h>
#include "music.h"
#include "database.h"
#include "interface.h"
#include "player.h"
#include "utils.h"


GtkTreeView *artist_tree;
GtkListStore *artist_store;

GtkTreeView *album_tree;
GtkListStore *album_store;

GtkTreeView *track_tree;
GtkListStore *track_store;

enum 
{
	COL_ARTIST_NAME,
	COL_ARTIST_ID,
	COLS_ARTIST
};


enum 
{
	COL_ALBUM_NAME,
	COL_ALBUM_ID,
	COLS_ALBUM
};


enum 
{
	COL_TRACK_NR,
	COL_TRACK_TITLE,
	COL_TRACK_ARTIST,
	COL_TRACK_ALBUM,
	COL_TRACK_ID,
	COLS_TRACK
};


// Prototypen
void music_artist_setup_tree (void);
void music_album_setup_tree (void);
void music_track_setup_tree (void);

gint sort_artist_compare_func (GtkTreeModel *model, GtkTreeIter *a,
							   GtkTreeIter *b, gpointer userdata);
							   
gint sort_album_compare_func (GtkTreeModel *model, GtkTreeIter *a,
							   GtkTreeIter *b, gpointer userdata);
							   
void music_artist_insert (const gint id, const gchar *artist);
void music_album_insert (const gint id, const gchar *album);
void music_track_insert (gchar *track, gchar *album, gchar *artist);

void artistname_cell_data_cb (GtkTreeViewColumn *tree_column,
							GtkCellRenderer *cell,
							GtkTreeModel *tree_model,
							GtkTreeIter *iter,
							gpointer data);

void music_artist_fill (void);


// Initialisierung
void music_init (void)
{
	g_message ("\tMusic Modul init");
	
	// TreeViews erstellen
	music_artist_setup_tree ();
	music_album_setup_tree ();
	music_track_setup_tree ();
	
	// Alle Artisten einfügen
	music_artist_fill ();
}


void music_artist_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSortable *sortable;
	
	// TreeView holen
	artist_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_artists");
	if (artist_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_artists nicht holen!");
	}

	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Interpret", renderer,
													   "text", COL_ARTIST_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_ARTIST_NAME);
	gtk_tree_view_append_column (GTK_TREE_VIEW (artist_tree), column);
											
	// Store erstellen
	artist_store = gtk_list_store_new (COLS_ARTIST, G_TYPE_STRING, G_TYPE_INT);
	
	// Sortierung
	sortable = GTK_TREE_SORTABLE (artist_store);
	gtk_tree_sortable_set_sort_func (sortable, COL_ARTIST_NAME, sort_artist_compare_func,
                                     GINT_TO_POINTER(COL_ARTIST_NAME), NULL);
    gtk_tree_sortable_set_sort_column_id (sortable, COL_ARTIST_NAME, GTK_SORT_ASCENDING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (artist_tree),
							 GTK_TREE_MODEL (artist_store));				 
}


void music_album_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSortable *sortable;
	
	// TreeView holen
	album_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_albums");
	if (album_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_albums nicht holen!");
	}
	//gtk_tree_view_set_headers_visible (album_tree, FALSE);

	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Album", renderer,
													   "text", COL_ALBUM_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_ALBUM_NAME);
	gtk_tree_view_append_column (GTK_TREE_VIEW (album_tree), column);
											
	// Store erstellen
	album_store = gtk_list_store_new (COLS_ALBUM, G_TYPE_STRING, G_TYPE_INT);
	
	// Sortierung
	sortable = GTK_TREE_SORTABLE (album_store);
	gtk_tree_sortable_set_sort_func (sortable, COL_ALBUM_NAME, sort_album_compare_func,
                                     GINT_TO_POINTER(COL_ALBUM_NAME), NULL);
    gtk_tree_sortable_set_sort_column_id (sortable, COL_ALBUM_NAME, GTK_SORT_ASCENDING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (album_tree),
							 GTK_TREE_MODEL (album_store));						 
}


// TreeView für die Tracks erstellen
void music_track_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSortable *sortable;
	
	// TreeView holen
	track_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_tracks");
	if (track_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_tracks nicht holen!");
	}
	//gtk_tree_view_set_headers_visible (track_tree, FALSE);

	// Track Nummer
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Nr.", renderer,
													   "text", COL_TRACK_NR, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_TRACK_NR);
	gtk_tree_view_append_column (GTK_TREE_VIEW (track_tree), column);
	
	// Titel
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Titel", renderer,
													   "text", COL_TRACK_TITLE, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_TRACK_TITLE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (track_tree), column);
	
	// Artist
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Artist", renderer,
													   "text", COL_TRACK_ARTIST, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_TRACK_ARTIST);
	gtk_tree_view_append_column (GTK_TREE_VIEW (track_tree), column);
	
	// Album
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Album", renderer,
													   "text", COL_TRACK_ALBUM, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_TRACK_ALBUM);
	gtk_tree_view_append_column (GTK_TREE_VIEW (track_tree), column);
	
											
	// Store erstellen
	track_store = gtk_list_store_new (COLS_TRACK, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
	
	// Sortierung
	sortable = GTK_TREE_SORTABLE (track_store);
    gtk_tree_sortable_set_sort_column_id (sortable, COL_TRACK_NR, GTK_SORT_ASCENDING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (track_tree),
							 GTK_TREE_MODEL (track_store));						 
}

// Sortierungsfunktion für Artist
gint sort_artist_compare_func (GtkTreeModel *model,
							   GtkTreeIter  *a,
							   GtkTreeIter  *b,
							   gpointer      userdata)
{
	gint ret = 0;
	gchar *name1, *name2;

	gtk_tree_model_get (model, a, COL_ARTIST_NAME, &name1, -1);
	gtk_tree_model_get (model, b, COL_ARTIST_NAME, &name2, -1);

	if (name1 == NULL || name2 == NULL) {
		if (name1 == NULL && name2 == NULL) {
			ret = 0;
		} else if (name1 == NULL) {
			ret = -1;
		} else {
			ret = 1;
		}
	} else {
		ret = g_utf8_collate (name1, name2);
	}

	g_free(name1);
	g_free(name2);
	
	return ret;
}


// Sortierungsfunktion für Album
gint sort_album_compare_func (GtkTreeModel *model,
							 GtkTreeIter  *a,
							 GtkTreeIter  *b,
							 gpointer      userdata)
{
	gint ret = 0;
	gchar *name1, *name2;

	gtk_tree_model_get (model, a, COL_ALBUM_NAME, &name1, -1);
	gtk_tree_model_get (model, b, COL_ALBUM_NAME, &name2, -1);

	if (name1 == NULL || name2 == NULL) {
		if (name1 == NULL && name2 == NULL) {
			ret = 0;
		} else if (name1 == NULL) {
			ret = -1;
		} else {
			ret = 1;
		}
	} else {
		ret = g_utf8_collate (name1, name2);
	}

	g_free(name1);
	g_free(name2);
	
	return ret;
}


void music_artist_insert (const gint id, const gchar *artist)
{
	GtkTreeIter iter;
	
	gtk_list_store_append (artist_store, &iter);
	gtk_list_store_set (artist_store, &iter, COL_ARTIST_ID, id,
											 COL_ARTIST_NAME, artist, -1);
}


void music_album_insert (const gint id, const gchar *album)
{
	GtkTreeIter iter;
	
	gtk_list_store_append (album_store, &iter);
	gtk_list_store_set (album_store, &iter, COL_ALBUM_ID, id,
											COL_ALBUM_NAME, album, -1);		
}


static gint artist_callback (void *NotUsed, gint rows, gchar **cols, gchar **titles)
{
	g_debug ("DB Artist Insert: Id=%i / Name=%s", atoi(cols[0]), cols[1]);
	
	music_artist_insert (atoi (cols[0]), cols[1]);

	return 0;
}



static gint album_callback (void *NotUsed, gint rows, gchar **cols, gchar **titles)
{
	g_debug ("DB Album Insert: Id=%i / Name=%s", atoi(cols[2]), cols[3]);
	
	music_album_insert (atoi (cols[2]), cols[3]);

	return 0;
}



static gint track_callback (void *NotUsed, gint rows, gchar **cols, gchar **titles)
{
	g_debug ("DB Track Insert: Id=%i / Name=%s", atoi(cols[0]), cols[4]);
	
	GtkTreeIter iter;

	gtk_list_store_append (track_store, &iter);
	gtk_list_store_set (track_store, &iter, COL_TRACK_NR, 1,
											COL_TRACK_TITLE, cols[4],
											COL_TRACK_ARTIST, cols[5],
											COL_TRACK_ALBUM, cols[6], -1);

	return 0;
}




// Fülle alle Artisten ein
void music_artist_fill (void)
{
	/*GDir *dir;
	const gchar *dirname;

	if ((dir = g_dir_open (get_music_dir (), 0, NULL))) {
		while ((dirname = g_dir_read_name (dir))) {
			g_assert (strcmp (dirname, ".") != 0);
			g_assert (strcmp (dirname, "..") != 0);
			if (strcmp (dirname, ".") == 0 || strcmp (dirname, "..") == 0) {
				continue;
			}

			music_artist_insert (dirname);
		}
		g_dir_close (dir);
	}*/
	
	
	// Datenbankzugriff
	db_execute_sql ("SELECT * FROM tbl_artist", artist_callback);
	
	
}




// Callback Funktionen

// Tritt ein, wenn auf dem Artist TreeView eine Zeile markiert wird
void on_treeview_artists_cursor_changed (GtkTreeView *tree, gpointer user_data)
{
	GtkTreePath *path;
	GtkTreeViewColumn *col;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *name;
	
	gtk_tree_view_get_cursor (tree, &path, &col);
	model = gtk_tree_view_get_model (tree);
	
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, COL_ARTIST_NAME, &name, -1);
	
	//g_debug ("Artist cursor changed: row=%s", name);
	
	gtk_tree_view_row_activated (tree, path, col);
}


// Tritt ein, wenn auf dem Album TreeView eine Zeile markiert wird
void on_treeview_albums_cursor_changed (GtkTreeView *tree, gpointer user_data)
{
	GtkTreePath *path;
	GtkTreeViewColumn *col;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *album;
	
	gtk_tree_view_get_cursor (tree, &path, &col);
	model = gtk_tree_view_get_model (tree);
	
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, COL_ALBUM_NAME, &album, -1);
	
	g_debug ("Album cursor changed: row=%s", album);
	
	gtk_tree_view_row_activated (tree, path, col);
}


// Tritt ein, wenn auf dem Artist TreeView eine Zeile aktiviert wird
void on_treeview_artists_row_activated (GtkTreeView *tree,
										 GtkTreePath *path,
										 GtkTreeViewColumn *column,
										 gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint id;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, COL_ARTIST_ID, &id, -1);
	
	g_debug ("Id Artist %d", id);
	
	
	// Vorhandene Alben zuerst löschen
	gtk_list_store_clear (album_store);
	
	
	gchar *sql;
	sql = g_strdup_printf ("SELECT * FROM view_artist_album WHERE IDartist = %d", id);
	
	db_execute_sql (sql , album_callback);
	
}


// Tritt ein, wenn auf dem Album TreeView eine Zeile aktiviert wird
void on_treeview_albums_row_activated (GtkTreeView *tree,
										 GtkTreePath *path,
										 GtkTreeViewColumn *column,
										 gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint id;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, COL_ALBUM_ID, &id, -1);
	
	// Vorhandene Tracks zuerst löschen
	gtk_list_store_clear (track_store);
	
	// Hier Tracks abfüllen	
	gchar *sql;
	sql = g_strdup_printf ("SELECT * FROM view_track_tree WHERE IDalbum = %d", id);
	
	db_execute_sql (sql , track_callback);
}


// Tritt ein, wenn auf dem Track TreeView eine Zeile aktiviert wird
void on_treeview_tracks_row_activated (GtkTreeView *tree,
										 GtkTreePath *path,
										 GtkTreeViewColumn *column,
										 gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *track_path;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	
	// Hier Track-Details holen
	
	
	
	
	//gtk_tree_model_get (model, &iter, STORE_TRACK_PATH, &track_path, -1);
	
	// Musik abspielen
	track_path = g_strdup_printf ("file://%s", track_path);
	player_play_uri (track_path);
	player_play_from_list (model, path);
}


