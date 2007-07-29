/*
 *      radio.c
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

#include <gtk/gtk.h>
#include <glade/glade.h>

#include <stdio.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-utils.h>

#include "radio.h"
#include "player.h"
#include "interface.h"



GtkTreeView *genre_tree;
GtkListStore *genre_store;
GtkTreeView *station_tree;
GtkListStore *station_store;

enum 
{ 
  COL_R_G_NAME,
  COLS_R_G
};

enum 
{ 
  COL_R_S_NAME,
  COL_R_S_BITRATE,
  COLS_R_S
};


// Prototypen
void radio_genre_setup_tree (void);
void radio_genre_insert (gchar *name);
void radio_genre_fetch_list (void);
void radio_station_setup_tree (void);
void radio_station_insert (gchar *name, gint bitrate);
gchar* radio_genre_load_xml (void);
int radio_print_error (GnomeVFSResult result, const char *uri_string);


void radio_init(void)
{
	g_message ("\tRadio Modul init");
	
	// Die beiden Trees aufbereiten
	radio_genre_setup_tree ();
	radio_station_setup_tree ();
	
	// Genres holen
	//radio_genre_fetch_list ();
}

void on_button_radio_stream_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Radio Stream...\n");
	
	GtkWidget *urlinput;
	urlinput = glade_xml_get_widget(glade, "entry_radio_url");
	
	player_play_uri(gtk_entry_get_text(GTK_ENTRY(urlinput)));
}


static void xml_start(GMarkupParseContext *context,
					  const gchar *element_name,
					  const gchar **attribute_names,
					  const gchar **attribute_values,
					  gpointer data,
					  GError **error)
{
	const gchar *element;

	element = g_markup_parse_context_get_element(context);
	if (g_ascii_strcasecmp(element, "genre") == 0) {
		//g_debug("genre: %s", attribute_values[0]);
		radio_genre_insert ((gchar*)attribute_values[0]);
	}
}


static void xml_end(GMarkupParseContext *context,
					const gchar *element_name,
					gpointer data,
					GError **error)
{
	const gchar *element;

	element = g_markup_parse_context_get_element(context);
}


static void xml_element(GMarkupParseContext *context,
						const gchar *text,
						gsize text_len,
						gpointer data,
						GError **error)
{
  const gchar *element;

  element = g_markup_parse_context_get_element(context);  
}


/* Handle errors that occur in parsing the XML file. */
static void xml_err(GMarkupParseContext *context,
					GError *error,
					gpointer data)
{
	g_critical("%s", error->message);
}




void on_button_radio_fetch_station_clicked(GtkWidget *widget, gpointer user_data)
{
	radio_genre_fetch_list ();
}


void radio_genre_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	// TreeView holen
	genre_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_radio_genre");
	if (genre_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_radio_genre nicht holen!");
	}
	//gtk_tree_view_set_headers_visible (genre_tree, FALSE);

	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Genre", renderer,
													   "text", COL_R_G_NAME, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (genre_tree), column);
											
	// Store erstellen
	genre_store = gtk_list_store_new (COLS_R_G, G_TYPE_STRING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (genre_tree),
							 GTK_TREE_MODEL (genre_store));						 
}


void radio_station_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	// TreeView holen
	station_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_radio_station");
	if (station_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_radio_station nicht holen!");
	}
	//gtk_tree_view_set_headers_visible (genre_tree, FALSE);

	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Sender", renderer,
													   "text", COL_R_S_NAME, NULL);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (station_tree), column);
	
	// Bitrate
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Bitrate", renderer,
													   "text", COL_R_S_BITRATE, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (station_tree), column);
											
	// Store erstellen
	station_store = gtk_list_store_new (COLS_R_S, G_TYPE_STRING, G_TYPE_INT);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (station_tree),
							 GTK_TREE_MODEL (station_store));						 
}


void radio_genre_insert (gchar *name)
{
	GtkTreeIter iter;
	
	g_debug ("radio_insert_genre: name=%s", name);
	
	genre_store = (GtkListStore*) gtk_tree_view_get_model (genre_tree);
	
	gtk_list_store_append (genre_store, &iter);
	gtk_list_store_set (genre_store, &iter,
						COL_R_G_NAME, name, -1);		
}


void radio_genre_fetch_list (void)
{
	g_debug("Fetch radio stations...");
	
	gchar *content;
	
	content = radio_genre_load_xml ();
  	
	static GMarkupParser parser = { xml_start, xml_end, xml_element, NULL, xml_err };
	GMarkupParseContext *context;
	
	context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
	g_markup_parse_context_parse (context, content, -1, NULL);
	g_markup_parse_context_free (context);
	
	g_free (content);
}


void radio_station_insert (gchar *name, gint bitrate)
{
	GtkTreeIter iter;
	
	g_debug ("radio_station_insert: name=%s, bitrate=%i", name, bitrate);
	
	station_store = (GtkListStore*) gtk_tree_view_get_model (station_tree);
	
	gtk_list_store_append (station_store, &iter);
	gtk_list_store_set (station_store, &iter,
						COL_R_S_NAME, name,
						COL_R_S_BITRATE, bitrate, -1);		
}


void on_treeview_radio_genre_row_activated (GtkTreeView *tree,
											GtkTreePath *path,
											GtkTreeViewColumn *column,
											gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *name;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, COL_R_G_NAME, &name, -1);
	
	g_debug ("Row activated: column=%s", name);
	radio_station_insert (name, 192);
}


gchar* radio_genre_load_xml (void)
{
	GnomeVFSResult result;
	
	gint file_size;
	gchar *uri = "http://www.shoutcast.com/sbin/newxml.phtml";
	GString *content;
	
	content = g_string_new_len ("", 20000);

	/* remember to initialize GnomeVFS! */
	if (!gnome_vfs_initialized ()) {
		if (!gnome_vfs_init ()) {
			printf ("Could not initialize GnomeVFS\n");
			return NULL;
		}
	}

	result = gnome_vfs_read_entire_file (uri, &file_size, &content->str);
	if (result != GNOME_VFS_OK) {
		radio_print_error (result, uri);
		return NULL;
	}

	return content->str;
}


int radio_print_error (GnomeVFSResult result, const char *uri_string)
{
  const char *error_string;
  /* get the string corresponding to this GnomeVFSResult value */
  error_string = gnome_vfs_result_to_string (result);
  printf ("Error %s occured opening location %s\n", error_string, uri_string);
  return 1;
}
