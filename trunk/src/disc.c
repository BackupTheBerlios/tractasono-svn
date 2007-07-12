/*
 *      disc.c
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

#include "disc.h"
#include "interface.h"


/**
 * The GtkTreeModel which all of the tracks are stored in
 */
//GtkWidget *track_store;


void disc_init()
{
	
	g_message ("\tDisc Modul init");
	
	
	
	treeview = glade_xml_get_widget(glade, "treeview_disc");
	if (treeview == NULL) {
		g_print("Fehler: Konnte treeview_disc nicht holen!\n");
	}
	
	
	track_store = gtk_list_store_new (COLUMN_TOTAL, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (track_store));
	{
		GtkTreeViewColumn *column;
		GtkCellRenderer *renderer;

		toggle_renderer = gtk_cell_renderer_toggle_new ();
		g_signal_connect (toggle_renderer, "toggled", G_CALLBACK (on_extract_toggled), NULL);
		column = gtk_tree_view_column_new_with_attributes ("",
											   toggle_renderer,
											   "active", COLUMN_EXTRACT,
											   NULL);
		gtk_tree_view_column_set_resizable (column, FALSE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		column = gtk_tree_view_column_new ();
		gtk_tree_view_column_set_title (column, "Track");
		gtk_tree_view_column_set_expand (column, FALSE);
		gtk_tree_view_column_set_resizable (column, FALSE);
		renderer = gtk_cell_renderer_text_new ();
		gtk_tree_view_column_pack_start (column, renderer, FALSE);
		gtk_tree_view_column_add_attribute (column, renderer, "text", COLUMN_NUMBER);
		renderer = gtk_cell_renderer_pixbuf_new ();
		g_object_set (renderer, "stock-size", GTK_ICON_SIZE_MENU, "xalign", 0.0, NULL);
		gtk_tree_view_column_pack_start (column, renderer, TRUE);
		gtk_tree_view_column_set_cell_data_func (column, renderer, number_cell_icon_data_cb, NULL, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		title_renderer = gtk_cell_renderer_text_new ();
		g_signal_connect (title_renderer, "edited", G_CALLBACK (on_cell_edited), GUINT_TO_POINTER (COLUMN_TITLE));
		g_object_set (G_OBJECT (title_renderer), "editable", TRUE, NULL);
		column = gtk_tree_view_column_new_with_attributes ("Title",
											   title_renderer,
											   "text", COLUMN_TITLE,
											   NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_expand (column, TRUE);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		artist_renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Artist",
											   artist_renderer,
											   "text", COLUMN_ARTIST,
											   NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_expand (column, TRUE);
		g_signal_connect (artist_renderer, "edited", G_CALLBACK (on_cell_edited), GUINT_TO_POINTER (COLUMN_ARTIST));
		g_object_set (G_OBJECT (artist_renderer), "editable", TRUE, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Duration",
											   renderer,
											   NULL);
		gtk_tree_view_column_set_resizable (column, FALSE);
		gtk_tree_view_column_set_cell_data_func (column, renderer, duration_cell_data_cb, NULL, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	}
	
	
	
	
	
	
	
}




void on_extract_toggled (GtkCellRendererToggle *cellrenderertoggle,
                                gchar *path,
                                gpointer user_data)
{
	g_print("Extract...\n");
	
}





void number_cell_icon_data_cb (GtkTreeViewColumn *tree_column,
				      GtkCellRenderer *cell,
				      GtkTreeModel *tree_model,
				      GtkTreeIter *iter,
				      gpointer data)
{
  TrackState state;
  gtk_tree_model_get (tree_model, iter, COLUMN_STATE, &state, -1);
  switch (state) {
  case STATE_IDLE:
    g_object_set (G_OBJECT (cell), "stock-id", "", NULL);
    break;
  case STATE_PLAYING:
    //g_object_set (G_OBJECT (cell), "stock-id", SJ_STOCK_PLAYING, NULL);
    break;
  case STATE_EXTRACTING:
    //g_object_set (G_OBJECT (cell), "stock-id", SJ_STOCK_RECORDING, NULL);
    break;
  default:
    g_warning("Unhandled track state %d\n", state);
  }
}


/**
 * Callback when the title or artist cells are edited in the list. column_data
 * contains the column number in the model which was modified.
 */
void on_cell_edited (GtkCellRendererText *renderer,
                 gchar *path, gchar *string,
                 gpointer column_data)
{
  ViewColumn column = GPOINTER_TO_INT (column_data);
  GtkTreeIter iter;
  TrackDetails *track;
  char *artist, *title;

  if (!gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (track_store), &iter, path))
    return;
  gtk_tree_model_get (GTK_TREE_MODEL (track_store), &iter,
                      COLUMN_ARTIST, &artist,
                      COLUMN_TITLE, &title,
                      COLUMN_DETAILS, &track,
                      -1);
  switch (column) {
  case COLUMN_TITLE:
    g_free (track->title);
    track->title = g_strdup (string);
    gtk_list_store_set (track_store, &iter, COLUMN_TITLE, track->title, -1);
    break;
  case COLUMN_ARTIST:
    g_free (track->artist);
    track->artist = g_strdup (string);
    gtk_list_store_set (track_store, &iter, COLUMN_ARTIST, track->artist, -1);
    break;
  default:
    g_warning ("Unknown column %d was edited", column);
  }
  g_free (artist);
  g_free (title);
  return;
}



/**
 * GtkTreeView cell renderer callback to render durations
 */
void duration_cell_data_cb (GtkTreeViewColumn *tree_column,
                                GtkCellRenderer *cell,
                                GtkTreeModel *tree_model,
                                GtkTreeIter *iter,
                                gpointer data)
{
  int duration;
  char *string;

  gtk_tree_model_get (tree_model, iter, COLUMN_DURATION, &duration, -1);
  if (duration != 0) {
    string = g_strdup_printf("%d:%02d", duration / 60, duration % 60);
  } else {
    string = g_strdup("(unknown)");
  }
  g_object_set (G_OBJECT (cell), "text", string, NULL);
  g_free (string);
}


void disc_set_disctitle (gchar *title)
{
	GtkWidget *label;
	
	label = glade_xml_get_widget (glade, "label_disctitle");
	if (!label) {
		g_error ("Konnte label_disctitle nicht holen!");
	}
	
	gtk_label_set_text (GTK_LABEL (label), title);
}
