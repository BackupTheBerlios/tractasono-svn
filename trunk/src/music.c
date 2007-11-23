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
#include "music.h"
#include "database.h"
#include "interface.h"


GtkTreeView *artist_tree;
GtkListStore *artist_store;

GtkTreeView *album_tree;
GtkListStore *album_store;


enum 
{ 
	COL_ARTIST_NAME,
	COLS_ARTIST
};

enum
{
	STORE_ARTIST_NAME,
	STORE_ARTIST_PATH
};


enum 
{ 
	COL_ALBUM_NAME,
	COLS_ALBUM
};

enum
{
	STORE_ALBUM_NAME,
	STORE_ALBUM_PATH
};


// Prototypen
void music_artist_setup_tree (void);
void music_album_setup_tree (void);

gint sort_artist_compare_func (GtkTreeModel *model, GtkTreeIter *a,
							   GtkTreeIter *b, gpointer userdata);
							   
gint sort_album_compare_func (GtkTreeModel *model, GtkTreeIter *a,
							   GtkTreeIter *b, gpointer userdata);
							   
void music_artist_insert (const gchar *name);
void music_album_insert (const gchar *name);

void music_artist_fill (void);

gchar *get_music_dir (void);
gchar *get_artist_dir (const gchar *artist);
gchar *get_album_dir (const gchar *album, const gchar *artist);


// Initialisierung
void music_init (void)
{
	g_message ("\tMusic Modul init");
	
	music_artist_setup_tree ();
	music_album_setup_tree ();
	
	
	
	music_artist_fill ();
	
	
	
	
	
	
	
	
	/*GtkWidget *container, *grid;

	// Genre
	container = glade_xml_get_widget(glade, "vbox_genre");
	grid = gnome_db_grid_new(database_get_genre_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	// Artist
	container = glade_xml_get_widget(glade, "vbox_interpret");
	grid = gnome_db_grid_new(database_get_artist_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	// Albums
	container = glade_xml_get_widget(glade, "vbox_album");
	grid = gnome_db_grid_new(database_get_album_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	// Song
	container = glade_xml_get_widget(glade, "vbox_track");
	grid = gnome_db_grid_new(database_get_song_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	*/
	
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
	//gtk_tree_view_set_headers_visible (artist_tree, FALSE);

	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Interpret", renderer,
													   "text", COL_ARTIST_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_ARTIST_NAME);
	gtk_tree_view_append_column (GTK_TREE_VIEW (artist_tree), column);
											
	// Store erstellen
	artist_store = gtk_list_store_new (COLS_ARTIST, G_TYPE_STRING);
	
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
	//gtk_tree_view_set_headers_visible (artist_tree, FALSE);

	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Album", renderer,
													   "text", COL_ALBUM_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_ALBUM_NAME);
	gtk_tree_view_append_column (GTK_TREE_VIEW (album_tree), column);
											
	// Store erstellen
	album_store = gtk_list_store_new (COLS_ALBUM, G_TYPE_STRING);
	
	// Sortierung
	sortable = GTK_TREE_SORTABLE (album_store);
	gtk_tree_sortable_set_sort_func (sortable, COL_ALBUM_NAME, sort_album_compare_func,
                                     GINT_TO_POINTER(COL_ALBUM_NAME), NULL);
    gtk_tree_sortable_set_sort_column_id (sortable, COL_ALBUM_NAME, GTK_SORT_ASCENDING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (album_tree),
							 GTK_TREE_MODEL (album_store));						 
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


void music_artist_insert (const gchar *name)
{
	GtkTreeIter iter;
	
	//g_debug ("music_artist_insert: name=%s", name);
	
	artist_store = (GtkListStore*) gtk_tree_view_get_model (artist_tree);
	
	gtk_list_store_append (artist_store, &iter);
	gtk_list_store_set (artist_store, &iter, COL_ARTIST_NAME, name, -1);		
}


void music_album_insert (const gchar *name)
{
	GtkTreeIter iter;
	
	//g_debug ("music_album_insert: name=%s", name);
	
	album_store = (GtkListStore*) gtk_tree_view_get_model (album_tree);
	
	gtk_list_store_append (album_store, &iter);
	gtk_list_store_set (album_store, &iter, COL_ALBUM_NAME, name, -1);		
}


// Fülle alle Artisten ein
void music_artist_fill (void)
{
	GDir *dir;
	const gchar *dirname;

	if ((dir = g_dir_open (get_music_dir (), 0, NULL))) {
		while ((dirname = g_dir_read_name (dir))) {
			/* This should be useless, but we'd better keep it for security */
			g_assert (strcmp (dirname, ".") != 0);
			g_assert (strcmp (dirname, "..") != 0);
			if (strcmp (dirname, ".") == 0 || strcmp (dirname, "..") == 0) {
				continue;
			}

			music_artist_insert (dirname);
		}
		g_dir_close (dir);
	}
	
}


// Gibt Default Musik Vezeichnis zurück
gchar *get_music_dir (void)
{
	return g_strdup_printf ("%s/tractasono/music/", g_get_home_dir());
}


// Gibt den Verzeichnisname für einen Artist zurück
gchar *get_artist_dir (const gchar *artist)
{
	return g_strdup_printf ("%s%s/", get_music_dir (), artist);
}


// Gibt den Verzeichnisname für einen Album zurück
gchar *get_album_dir (const gchar *album, const gchar *artist)
{
	return g_strdup_printf ("%s%s/%s/", get_music_dir (), artist, album);
}



// Callback Funktionen

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
	
	//g_debug ("Row activated: row=%s", name);
	
	gtk_tree_view_row_activated (tree, path, col);
	
}

void on_treeview_artists_row_activated (GtkTreeView *tree,
										 GtkTreePath *path,
										 GtkTreeViewColumn *column,
										 gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *name;
	
	g_debug ("row activated");
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, COL_ARTIST_NAME, &name, -1);
	
	// Vorhandene Alben zuerst löschen
	gtk_list_store_clear (album_store);
	
	GDir *dir;
	const gchar *dirname;

	if ((dir = g_dir_open (get_artist_dir (name), 0, NULL))) {
		while ((dirname = g_dir_read_name (dir))) {
			/* This should be useless, but we'd better keep it for security */
			g_assert (strcmp (dirname, ".") != 0);
			g_assert (strcmp (dirname, "..") != 0);
			if (strcmp (dirname, ".") == 0 || strcmp (dirname, "..") == 0) {
				continue;
			}

			music_album_insert (dirname);
		}
		g_dir_close (dir);
	}	
	
}


