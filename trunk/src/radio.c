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

#include <stdlib.h>

#include <gtk/gtk.h>
#include <glade/glade.h>

#include <stdio.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-utils.h>

#include <totem-pl-parser.h>

#include "radio.h"
#include "player.h"
#include "interface.h"
#include "lcd.h"
#include "gtk-circle.h"


const gchar url_genre[] = "http://www.shoutcast.com/sbin/newxml.phtml";
const gchar url_station[] = "http://www.shoutcast.com/sbin/newxml.phtml?genre=";
const gchar url_playlist[] = "http://www.shoutcast.com/sbin/shoutcast-playlist.pls?rn=";

GtkComboBoxEntry *combo;

TotemPlParser *pl_parser;

GtkTreeView *genre_tree;
GtkListStore *genre_store;
GtkTreeView *station_tree;
GtkListStore *station_store;


enum
{
	COL_URL_NAME,
	COLS_URL
};

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

enum
{
	STORE_S_NAME,
	STORE_S_BITRATE,
	STORE_S_ID,
	STORE_TOTAL
};


// Prototypen
void radio_genre_setup_tree (void);
void radio_station_setup_tree (void);

void radio_genre_insert_initial (void);

void radio_genre_insert (gchar *name);
void radio_station_insert (const gchar *name, const gchar *bitrate, const gchar *id);

void radio_genre_parse (void);
void radio_station_parse (const gchar *genre);

gint sort_genre_compare_func (GtkTreeModel *model,
							  GtkTreeIter  *a,
							  GtkTreeIter  *b,
							  gpointer      userdata);

gint sort_station_compare_func (GtkTreeModel *model,
								GtkTreeIter  *a,
								GtkTreeIter  *b,
								gpointer      userdata);

gchar* xml_load (const gchar *uri);

void cb_parser_start (TotemPlParser *parser, const char *title);
void cb_parser_end (TotemPlParser *parser, const char *title);
void cb_parser_entry (TotemPlParser *parser, const char *uri, const char *title,
				  const char *genre, gpointer data);
				  
int vfs_print_error (GnomeVFSResult result, const char *uri_string);

void setup_url_combo (void);
void radio_combo_add_url (const gchar *url);
void radio_combo_clean (void);


void radio_init(void)
{
	g_message ("\tRadio Modul init");
	
	// Die beiden Trees aufbereiten
	radio_genre_setup_tree ();
	radio_station_setup_tree ();
	
	// Genres einfügen
	radio_genre_insert_initial ();
	
	// Combo holen
	setup_url_combo ();
	
	// Playlist Parser einrichten
	pl_parser = totem_pl_parser_new ();
	g_object_set (pl_parser, "recurse", FALSE, "debug", FALSE, NULL);
	g_signal_connect (pl_parser, "playlist-start", G_CALLBACK(cb_parser_start), NULL);
	g_signal_connect (pl_parser, "playlist-end", G_CALLBACK(cb_parser_end), NULL);
	g_signal_connect (pl_parser, "entry-parsed", G_CALLBACK(cb_parser_entry), NULL);




	// Circle
	GtkCircle *circle;
	GtkWidget *vbox;
	vbox = glade_xml_get_widget(glade, "vbox_circle");
	if (vbox == NULL) {
		g_error("Konnte vbox_circle nicht holen!\n");
	}
	circle = gtk_circle_new ();
	gtk_container_add (GTK_CONTAINER (vbox), GTK_WIDGET (circle));
	
	// Widget anzeigen
	gtk_widget_show (GTK_WIDGET (circle));


}


void cb_parser_start (TotemPlParser *parser, const char *title)
{
	//g_message ("Playlist with name '%s' started", title);
}

void cb_parser_end (TotemPlParser *parser, const char *title)
{
	//g_message ("Playlist with name '%s' ended", title);
}


void cb_parser_entry (TotemPlParser *parser, const char *uri, const char *title,
				  const char *genre, gpointer data)
{
	//g_debug ("added URI '%s' with title '%s'", uri, title ? title : "empty");
	
	// GUI aktualisieren
	while (gtk_events_pending ()) {
		gtk_main_iteration ();
	}
	
	radio_combo_add_url (uri);
}


void on_button_radio_stream_clicked(GtkWidget *widget, gpointer user_data)
{
	//g_message ("Radio Stream abspielen...");
	
	GtkWidget *urlinput;
	const gchar *url;
	
	urlinput = glade_xml_get_widget(glade, "radio_url_combo_entry");
	url = gtk_entry_get_text(GTK_ENTRY(urlinput));
	
	// Stream abspielen
	player_play_uri(url);

	lcd_set_title (LCD(lcd), "Bla Bla");
}


static void xml_genre_start (GMarkupParseContext *context,
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


static void xml_station_start (GMarkupParseContext *context,
					  		   const gchar *element_name,
					  		   const gchar **attribute_names,
					  		   const gchar **attribute_values,
					  		   gpointer data,
					  		   GError **error)
{
	if (g_ascii_strcasecmp(element_name, "station") == 0) {
		/*g_debug("%s=%s // %s=%s // %s=%s // %s=%s // %s=%s", attribute_names[0], attribute_values[0],
											  attribute_names[1], attribute_values[1],
											  attribute_names[2], attribute_values[2],
											  attribute_names[3], attribute_values[3],
											  attribute_names[4], attribute_values[4]);*/
		radio_station_insert (attribute_values[0], attribute_values[3], attribute_values[2]);
	}
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
	radio_genre_parse ();
}


void radio_genre_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSortable *sortable;
	
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
	gtk_tree_view_column_set_sort_column_id (column, COL_R_G_NAME);
	gtk_tree_view_append_column (GTK_TREE_VIEW (genre_tree), column);
											
	// Store erstellen
	genre_store = gtk_list_store_new (COLS_R_G, G_TYPE_STRING);
	
	// Sortierung
	sortable = GTK_TREE_SORTABLE (genre_store);
	gtk_tree_sortable_set_sort_func (sortable, COL_R_G_NAME, sort_genre_compare_func,
                                     GINT_TO_POINTER(COL_R_G_NAME), NULL);
    gtk_tree_sortable_set_sort_column_id (sortable, COL_R_G_NAME, GTK_SORT_ASCENDING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (genre_tree),
							 GTK_TREE_MODEL (genre_store));						 
}


void radio_station_setup_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSortable *sortable;
	
	// TreeView holen
	station_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_radio_station");
	if (station_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_radio_station nicht holen!");
	}

	// Name
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, "ellipsize-set", TRUE, NULL);
	column = gtk_tree_view_column_new_with_attributes ("Sender", renderer,
													   "text", COL_R_S_NAME, NULL);
	gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width (column, 420);
	gtk_tree_view_column_set_sort_column_id (column, COL_R_S_NAME);
	gtk_tree_view_append_column (GTK_TREE_VIEW (station_tree), column);
	
	// Bitrate
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Bitrate", renderer,
													   "text", COL_R_S_BITRATE, NULL);
	gtk_tree_view_column_set_sort_column_id (column, COL_R_S_BITRATE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (station_tree), column);
											
	// Store erstellen
	station_store = gtk_list_store_new (STORE_TOTAL, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
	
	// Sortierung
	sortable = GTK_TREE_SORTABLE (station_store);
	gtk_tree_sortable_set_sort_func (sortable, COL_R_S_NAME, sort_station_compare_func,
                                     GINT_TO_POINTER(COL_R_S_NAME), NULL);
    gtk_tree_sortable_set_sort_func (sortable, COL_R_S_BITRATE, sort_station_compare_func,
                                     GINT_TO_POINTER(COL_R_S_BITRATE), NULL);                            
    gtk_tree_sortable_set_sort_column_id (sortable, COL_R_S_BITRATE, GTK_SORT_DESCENDING);	
										
	// Store dem Tree anhängen
	gtk_tree_view_set_model (GTK_TREE_VIEW (station_tree),
							 GTK_TREE_MODEL (station_store));						 
}


void radio_genre_insert (gchar *name)
{
	GtkTreeIter iter;
	
	//g_debug ("radio_insert_genre: name=%s", name);
	
	genre_store = (GtkListStore*) gtk_tree_view_get_model (genre_tree);
	
	gtk_list_store_append (genre_store, &iter);
	gtk_list_store_set (genre_store, &iter,
						COL_R_G_NAME, name, -1);		
}


// Beispiel Genres laden
void radio_genre_insert_initial (void)
{
	radio_genre_insert ("50s");
	radio_genre_insert ("60s");
	radio_genre_insert ("70s");
	radio_genre_insert ("80s");
	radio_genre_insert ("90s");
	radio_genre_insert ("Adult");
	radio_genre_insert ("Ambient");
	radio_genre_insert ("Blues");
	radio_genre_insert ("Breakbeat");
	radio_genre_insert ("Chillout");
	radio_genre_insert ("Charts");
	radio_genre_insert ("Classic");
	radio_genre_insert ("Club");
	radio_genre_insert ("Comedy");
	radio_genre_insert ("Country");
	radio_genre_insert ("Dance");
	radio_genre_insert ("Darkwave");
	radio_genre_insert ("Disco");
	radio_genre_insert ("EBM");
	radio_genre_insert ("Electro");
	radio_genre_insert ("Eurodance");
	radio_genre_insert ("Funk");
	radio_genre_insert ("Gothic");
	radio_genre_insert ("Hardcore");
	radio_genre_insert ("HipHop");
	radio_genre_insert ("Hits");
	radio_genre_insert ("House");
	radio_genre_insert ("Industrial");
	radio_genre_insert ("Jazz");
	radio_genre_insert ("Lounge");
	radio_genre_insert ("Metal");
	radio_genre_insert ("Minimal");
	radio_genre_insert ("News");
	radio_genre_insert ("Oldies");
	radio_genre_insert ("Pop");
	radio_genre_insert ("Punk");
	radio_genre_insert ("Reggae");
	radio_genre_insert ("Rock");
	radio_genre_insert ("Ska");
	radio_genre_insert ("Soul");
	radio_genre_insert ("Techno");
	radio_genre_insert ("Top40");
	radio_genre_insert ("Trance");
}


void radio_genre_parse (void)
{
	//g_debug("Fetch & parse genres...");
	
	gchar *content;
	
	content = xml_load (url_genre);
  	
	static GMarkupParser parser = { xml_genre_start, NULL, NULL, NULL, xml_err };
	GMarkupParseContext *context;
	
	context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
	g_markup_parse_context_parse (context, content, -1, NULL);
	g_markup_parse_context_free (context);
	
	g_free (content);
}


void radio_station_parse (const gchar *genre)
{
	//g_debug("Fetch & parse radio stations...");
	
	GString *s_genre;
	gchar *content;
	
	s_genre = g_string_new (url_station);
	g_string_append_printf (s_genre, "%s", genre);
	
	content = xml_load (s_genre->str);
  	
	static GMarkupParser parser = { xml_station_start, NULL, NULL, NULL, xml_err };
	GMarkupParseContext *context;
	
	context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
	g_markup_parse_context_parse (context, content, -1, NULL);
	g_markup_parse_context_free (context);
	
	g_free (content);
}


void radio_station_insert (const gchar *name, const gchar *bitrate, const gchar *id)
{
	GtkTreeIter iter;
	gint bitr = 0;
	
	bitr = atoi (bitrate);
	
	station_store = (GtkListStore*) gtk_tree_view_get_model (station_tree);
	
	gtk_list_store_append (station_store, &iter);
	gtk_list_store_set (station_store, &iter,
						STORE_S_NAME, name,
						STORE_S_BITRATE, bitr,
						STORE_S_ID, id, -1);		
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
	
	//g_debug ("Row activated: column=%s", name);
	
	// Vorhandene Stationen zuerst löschen
	gtk_list_store_clear (station_store);
	
	radio_station_parse (name);
}


void on_treeview_radio_station_row_activated (GtkTreeView *tree,
											  GtkTreePath *path,
											  GtkTreeViewColumn *column,
											  gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *name, *id;
	GString *s_url;
	TotemPlParserResult parser_result;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, STORE_S_NAME, &name, STORE_S_ID, &id, -1);
	
	//g_debug ("Station Row activated: name=%s, id=%s", name, id);
	
	s_url = g_string_new (url_playlist);
	g_string_append_printf (s_url, "%s", id);
	
	// Combo leeren
	radio_combo_clean ();
	
	// Hole Playlist des Senders
	//g_debug ("Playlist parsing beginnt!");
	parser_result = totem_pl_parser_parse (pl_parser, s_url->str, FALSE);
	//g_debug ("Playlist parsing beendet!");
	/*if (parser_result == TOTEM_PL_PARSER_RESULT_SUCCESS) {
		g_debug ("   Parsing war erfolgreich!");
	} else {
		g_debug ("   Parsing war nicht erfolgreich!");
	}*/
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

	GtkWidget *urlinput;
	const gchar *url;
	
	urlinput = glade_xml_get_widget(glade, "radio_url_combo_entry");
	url = gtk_entry_get_text(GTK_ENTRY(urlinput));

	lcd_set_title (LCD(lcd), NULL);
	lcd_set_artist (LCD(lcd), NULL);
	lcd_set_album (LCD(lcd), NULL);

	// Stream abspielen
	player_play_uri(url);
	
	lcd_set_title (LCD(lcd), name);
}


// Sortierungsfunktion für Genre
gint sort_genre_compare_func (GtkTreeModel *model,
							 GtkTreeIter  *a,
							 GtkTreeIter  *b,
							 gpointer      userdata)
{
	gint ret = 0;
	gchar *name1, *name2;

	gtk_tree_model_get (model, a, COL_R_G_NAME, &name1, -1);
	gtk_tree_model_get (model, b, COL_R_G_NAME, &name2, -1);

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


// Sortierungsfunktion für Station
gint sort_station_compare_func (GtkTreeModel *model,
								GtkTreeIter  *a,
								GtkTreeIter  *b,
								gpointer      userdata)
{
	gint sortcol = GPOINTER_TO_INT(userdata);
	gint ret = 0;

	switch (sortcol) {
		case COL_R_S_NAME: {
			gchar *name1, *name2;

			gtk_tree_model_get (model, a, COL_R_S_NAME, &name1, -1);
			gtk_tree_model_get (model, b, COL_R_S_NAME, &name2, -1);

			if (name1 == NULL || name2 == NULL) {
				if (name1 == NULL && name2 == NULL)
					break; /* both equal => ret = 0 */
					
				ret = (name1 == NULL) ? -1 : 1;
			} else {
				ret = g_utf8_collate(name1,name2);
			}

			g_free(name1);
			g_free(name2);
		}
		break;

		case COL_R_S_BITRATE: {
			gint bitrate1, bitrate2;

			gtk_tree_model_get (model, a, COL_R_S_BITRATE, &bitrate1, -1);
			gtk_tree_model_get (model, b, COL_R_S_BITRATE, &bitrate2, -1);

			if (bitrate1 != bitrate2) {
				ret = (bitrate1 > bitrate2) ? 1 : -1;
			} /* else both equal => ret = 0 */
		}
		break;
		
		default:
			g_return_val_if_reached (0);
	}

	return ret;
}


gchar* xml_load (const gchar *uri)
{
	GnomeVFSResult result;
	
	gint file_size;
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
		vfs_print_error (result, uri);
		return NULL;
	}

	return content->str;
}


int vfs_print_error (GnomeVFSResult result, const char *uri_string)
{
	const char *error_string;
	/* get the string corresponding to this GnomeVFSResult value */
	error_string = gnome_vfs_result_to_string (result);
	printf ("Error %s occured opening location %s\n", error_string, uri_string);
	return 1;
}


void setup_url_combo (void)
{
	GtkListStore *store;
	
	combo = (GtkComboBoxEntry*) glade_xml_get_widget (glade, "radio_url_combo");
	if (!combo) {
		g_warning ("Konnte URL Combo nicht holen!");
	}
	
	store = gtk_list_store_new (COLS_URL, G_TYPE_STRING);
	gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
	gtk_combo_box_entry_set_text_column (combo, COL_URL_NAME);
}


void radio_combo_add_url (const gchar *url)
{
	GtkListStore *store;
	GtkTreeIter iter;
	
	//g_debug ("radio_combo_add_url");
	
	store = (GtkListStore*) gtk_combo_box_get_model (GTK_COMBO_BOX(combo));
	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, COL_URL_NAME, url, -1);
	
}

void radio_combo_clean (void)
{
	GtkListStore *store;
	
	//g_debug ("radio_combo_clean");
	
	store = (GtkListStore*) gtk_combo_box_get_model (GTK_COMBO_BOX(combo));
	gtk_list_store_clear (store);
}

