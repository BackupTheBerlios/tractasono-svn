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
#include "musicbrainz.h"
#include "interface.h"
#include "strukturen.h"


// GStreamer Variablen
GstElement *ripper;

// TreeView
GtkTreeView *disc_tree;


typedef enum {
	COLUMN_STATE,
	COLUMN_EXTRACT,
	COLUMN_NUMBER,
	COLUMN_TITLE,
	COLUMN_ARTIST,
	COLUMN_DURATION,
	COLUMN_DETAILS,
	COLUMN_TOTAL
} ViewColumn;

typedef enum {
	STATE_IDLE,
	STATE_PLAYING,
	STATE_EXTRACTING,
	STATE_IMPORTED
} TrackState;



// Prototypen
void setup_ripper_pipeline (void);
void ripper_test (void);
void display_disctitle (AlbumDetails *album);
void track_insert (const TrackDetails *track);
void track_setup_tree (void);
void extract_disc (void);
void extract_track (TrackDetails *track);


void disc_init()
{
	g_message ("\tDisc Modul init");
	
	// TreeView initialisieren
	track_setup_tree ();
	
	// Ripper Pipeline aufsetzen
	setup_ripper_pipeline ();

	// Testfunktion
	//ripper_test ();
	
}

void setup_ripper_pipeline (void)
{
	g_message ("\t\tSetup ripper pipeline...");
	
	/* elements */
	GstElement *source;
	GstElement *conv;
	GstElement *sink;

	/* create ripper pipeline */
	ripper = gst_pipeline_new ("ripper");
	if (ripper == NULL) {
		g_warning ("Ripper pipeline konnte nicht erstellt werden!");
	}

	/* create ripper elements */
	source = gst_element_factory_make ("cdparanoiasrc", "source");
	conv = gst_element_factory_make ("audioconvert", "converter");
	sink = gst_element_factory_make ("filesink", "sink");

	/* must add elements to pipeline before linking them */
	gst_bin_add_many (GST_BIN (ripper), source, conv, sink, NULL);

	/* link */
	if (!gst_element_link_many (source, conv, sink, NULL)) {
		g_warning ("Failed to link ripper elements!");
	}
	
}


void ripper_test (void)
{
	
	
}




void on_extract_toggled (GtkCellRendererToggle *cellrenderertoggle,
						 gchar *path, gpointer user_data)
{
	gboolean extract;
	GtkTreeIter iter;
	GtkListStore *store;
	
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);

	if (!gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (store), &iter, path)) {
		return;
	}
	
	gtk_tree_model_get (GTK_TREE_MODEL (store), &iter, COLUMN_EXTRACT, &extract, -1);
	
	/* extract is the old state here, so toggle */
	extract = !extract;
	gtk_list_store_set (store, &iter, COLUMN_EXTRACT, extract, -1);

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
			g_object_set (G_OBJECT (cell), "stock-id", "gtk-media-play", NULL);
			break;
		case STATE_EXTRACTING:
			g_object_set (G_OBJECT (cell), "stock-id", "gtk-media-record", NULL);
			break;
		case STATE_IMPORTED:
			g_object_set (G_OBJECT (cell), "stock-id", "gtk-apply", NULL);
			break;
		default:
			g_warning ("Unhandled track state %d", state);
	}
}



// Callback when the title or artist cells are edited in the list. column_data
// contains the column number in the model which was modified.
void on_cell_edited (GtkCellRendererText *renderer, gchar *path,
					 gchar *string, gpointer column_data)
{
	GtkListStore *store;
	
	ViewColumn column = GPOINTER_TO_INT (column_data);
	GtkTreeIter iter;
	TrackDetails *track;
	char *artist, *title;
	
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);

	if (!gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (store), &iter, path))
		return;
		
		gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
							COLUMN_ARTIST, &artist,
							COLUMN_TITLE, &title,
							COLUMN_DETAILS, &track, -1);
		switch (column) {
			case COLUMN_TITLE:
				g_free (track->title);
				track->title = g_strdup (string);
				gtk_list_store_set (store, &iter, COLUMN_TITLE, track->title, -1);
				break;
			case COLUMN_ARTIST:
				g_free (track->artist);
				track->artist = g_strdup (string);
				gtk_list_store_set (store, &iter, COLUMN_ARTIST, track->artist, -1);
				break;
			default:
				g_warning ("Unknown column %d was edited", column);
		}
		
	g_free (artist);
	g_free (title);
	
	return;
}




// GtkTreeView cell renderer callback to render durations
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



void display_disctitle (AlbumDetails *album)
{
	GtkWidget *label;
	gchar *text;
	
	label = glade_xml_get_widget (glade, "label_disctitle");
	if (!label) {
		g_error ("Konnte label_disctitle nicht holen!");
	}
	
	text = g_strdup_printf ("%s - %s", album->artist, album->title);
	
	gtk_label_set_text (GTK_LABEL (label), text);
}



void on_button_reread_clicked (GtkWidget *widget, gpointer user_data)
{
	AlbumDetails *album;
	
	GtkListStore *store;
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
	
	// Vorhandene Tracks zuerst löschen
	gtk_list_store_clear (store);
	
	album = lookup_cd ();
	if (album == NULL) {
		g_warning ("CD konnte nich eingelesen werden!");
	} else {
		g_message ("CD einlesen (title: %s | artist: %s | tracks: %d)", album->title,
														  album->artist,
														  album->number);

		// CD Namen anzeigen
		display_disctitle (album);

		// Tracks ins Grid einfüllen
		GList *tracks = g_list_first(album->tracks);
		while (tracks != NULL) {
			track_insert (tracks->data);
			tracks = tracks->next;
		}											  
		
	}
	
}




void on_button_disc_play_clicked (GtkWidget *widget, gpointer user_data)
{
	g_message ("Play Track");
	
	GtkTreePath *path;
	GtkTreeViewColumn *col;
	
	gtk_tree_view_get_cursor (disc_tree, &path, &col);
	gtk_tree_view_row_activated (disc_tree, path, col);
}


void on_button_record_clicked (GtkWidget *widget, gpointer user_data)
{
	g_message ("Aufnahme!");
	
	// Rippe die CD
	extract_disc ();
}


void track_insert (const TrackDetails *track)
{
	GtkListStore *store;
	GtkTreeIter iter;
	
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
	gtk_list_store_append (store, &iter);
											
	gtk_list_store_set (store, &iter,
						COLUMN_STATE, STATE_IDLE,
						COLUMN_EXTRACT, TRUE,
						COLUMN_NUMBER, track->number,
						COLUMN_TITLE, track->title,
						COLUMN_ARTIST, track->artist,
						COLUMN_DURATION, track->duration,
						COLUMN_DETAILS, track, -1);
											
}


// TreeView für die Tracks erstellen
void track_setup_tree (void)
{
	GtkListStore *store;
	GtkTreeViewColumn *column;
	//GtkTreeSortable *sortable;
	GtkCellRenderer *renderer;
	GtkCellRenderer *toggle_renderer;
	GtkCellRenderer *title_renderer;
	GtkCellRenderer *artist_renderer;
	
	// TreeView holen
	disc_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_disc");
	if (disc_tree == NULL) {
		g_warning ("Fehler: Konnte treeview_tracks nicht holen!");
	}
	
	// Toggle
	toggle_renderer = gtk_cell_renderer_toggle_new ();
	g_signal_connect (toggle_renderer, "toggled", G_CALLBACK (on_extract_toggled), NULL);
	column = gtk_tree_view_column_new_with_attributes ("", toggle_renderer,
													   "active", COLUMN_EXTRACT, NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (disc_tree), column);
	
	
	// Spezialfeld (Track Nr + Indikator)
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
	gtk_tree_view_append_column (GTK_TREE_VIEW (disc_tree), column);
	
	
	// Titel
	title_renderer = gtk_cell_renderer_text_new ();
	g_signal_connect (title_renderer, "edited", G_CALLBACK (on_cell_edited), GUINT_TO_POINTER (COLUMN_TITLE));
	g_object_set (G_OBJECT (title_renderer), "editable", TRUE, NULL);
	column = gtk_tree_view_column_new_with_attributes ("Title",
										   title_renderer,
										   "text", COLUMN_TITLE,
										   NULL);
	gtk_tree_view_column_set_resizable (column, TRUE);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (disc_tree), column);
	
	
	// Artist
	artist_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Artist",
										   artist_renderer,
										   "text", COLUMN_ARTIST,
										   NULL);
	gtk_tree_view_column_set_resizable (column, TRUE);
	gtk_tree_view_column_set_expand (column, TRUE);
	g_signal_connect (artist_renderer, "edited", G_CALLBACK (on_cell_edited), GUINT_TO_POINTER (COLUMN_ARTIST));
	g_object_set (G_OBJECT (artist_renderer), "editable", TRUE, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (disc_tree), column);
	
	
	
	// Duration
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Duration",
										   renderer,
										   "text", COLUMN_DURATION,
										   NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_cell_data_func (column, renderer, duration_cell_data_cb, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (disc_tree), column);
	
	
											
	// Store erstellen
	store = gtk_list_store_new (COLUMN_TOTAL, G_TYPE_INT, G_TYPE_BOOLEAN,
								G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
								G_TYPE_INT, G_TYPE_POINTER);
	
	// Sortierung
	//sortable = GTK_TREE_SORTABLE (track_store);
    //gtk_tree_sortable_set_sort_column_id (sortable, STORE_TRACK_NR, GTK_SORT_ASCENDING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (disc_tree),
							 GTK_TREE_MODEL (store));	
							 
						 
}




/**
 * Foreach callback to populate pending with the list of TrackDetails to
 * extract.
 */
static gboolean extract_track_cb (GtkTreeModel *model, GtkTreePath *path,
								  GtkTreeIter *iter, gpointer data)
{
	gboolean extract;
	TrackDetails *track;

	gtk_tree_model_get (model, iter, COLUMN_EXTRACT, &extract,
									 COLUMN_DETAILS, &track, -1);
	if (extract) {
		gtk_list_store_set (GTK_LIST_STORE (model), iter, COLUMN_STATE, STATE_EXTRACTING, -1);
		extract_track (track);
		gtk_list_store_set (GTK_LIST_STORE (model), iter, COLUMN_STATE, STATE_IMPORTED, -1);
		gtk_list_store_set (GTK_LIST_STORE (model), iter, COLUMN_EXTRACT, FALSE, -1);
	}

	return FALSE;
}



void extract_track (TrackDetails *track)
{
	g_message ("Rippe Track: %d %s - %s", track->number, track->artist, track->title);
	
	
	
}



void extract_disc (void)
{
	GtkListStore *store;
	
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
	gtk_tree_model_foreach (GTK_TREE_MODEL (store), extract_track_cb, NULL);
	
	
	
}



// Tritt ein, wenn auf dem Track TreeView eine Zeile aktiviert wird
void on_treeview_disc_row_activated (GtkTreeView *tree,
									 GtkTreePath *path,
									 GtkTreeViewColumn *column,
									 gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *uri;
	gint number;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, COLUMN_NUMBER, &number, -1);
	
	// Musik abspielen
	uri = g_strdup_printf ("cdda://%d", number);
	player_play_uri (uri);
}



