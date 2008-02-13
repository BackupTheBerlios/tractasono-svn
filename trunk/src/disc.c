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
#include "utils.h"

#include <gconf/gconf-client.h>
#include <profiles/gnome-media-profiles.h>


// GStreamer Variablen
GstElement *ripper;
GstElement *source;
GstElement *encoder;
GstElement *sink;


AlbumDetails *the_album;

gint track_extracting;

static int total_extracting;


static GtkTreeIter current;

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
void ripper_setup (void);
void ripper_test (void);
void display_disctitle (AlbumDetails *album);
void track_insert (const TrackDetails *track);
void track_setup_tree (void);
void extract_disc (void);
void extract_track (TrackDetails *track);
void play_track (TrackDetails *track);
gboolean ripper_bus_callback (GstBus *bus, GstMessage *message, gpointer data);
static GstElement* build_encoder (void);
void display_track_state (gint track, TrackState state);
static gboolean find_next (void);
static void on_error_cb (GstBus *bus, GError *error, gpointer data);
static void on_completion_cb (GstBus *bus, GError *error, gpointer data);
static void on_progress_cb (GstBus *bus, GError *error, gpointer data);
gboolean get_compilation (void);
void set_compilation (gboolean active);


// Initialisation
void disc_init()
{
	g_message ("\tDisc Modul init");
	
	// TreeView initialisieren
	track_setup_tree ();
	
	// GConf & Media Profile initialisieren
	gnome_media_profiles_init (gconf_client_get_default ());
	
	// Ripper Pipeline aufsetzen
	ripper_setup ();

	// Variablen initialisieren
	the_album = NULL;

	// Testfunktion
	//ripper_test ();
	
	track_extracting = 1;
}

void ripper_setup (void)
{
	g_message ("\t\tSetup ripper pipeline");
	
	// Elemente
	GstElement *queue;
	GstElement *converter;
	GstElement *muxer;
	GstElement *tagger;
	
	// Bus
	GstBus *bus;

	/* create ripper pipeline */
	ripper = gst_pipeline_new ("ripper");
	if (ripper == NULL) {
		g_warning ("Konnte ripper pipeline nicht erstellen!");
	}
	
	// Source (Audio-CD)
	source = gst_element_factory_make ("cdparanoiasrc", "ripper-source");
	if (source == NULL) {
		g_warning ("Konnte source element nicht erstellen!");
	}
	
	// Warteschlange (grosser Buffer)
	queue = gst_element_factory_make ("queue", "ripper-queue");
	if (queue == NULL) {
		g_warning ("Konnte queue element nicht erstellen!");
	} else {
		g_object_set (queue, "max-size-time", 120 * GST_SECOND, NULL);
	}
	
	// Audio converter
	converter = gst_element_factory_make ("audioconvert", "ripper-converter");
	if (converter == NULL) {
		g_warning ("Konnte converter element nicht erstellen!");
	}
	
	// Encoder (Ogg Vorbis)
	encoder = gst_element_factory_make ("vorbisenc", "ripper-encoder");
	if (encoder == NULL) {
		g_warning ("Konnte encoder element nicht erstellen!");
	} else {
		g_object_set (encoder, "quality", 0.5, NULL);
	}
	
	// Muxer (Ogg Muxer)
	muxer = gst_element_factory_make ("oggmux", "ripper-muxer");
	if (muxer == NULL) {
		g_warning ("Konnte muxer element nicht erstellen!");
	}
	
	// Tagger (Vorbistag)
	tagger = gst_element_factory_make ("vorbistag", "ripper-tagger");
	if (tagger == NULL) {
		g_warning ("Konnte tagger element nicht erstellen!");
	}
	
	// Sink (Datei)
	sink = gst_element_factory_make ("filesink", "ripper-sink");
	if (sink == NULL) {
		g_warning ("Konnte sink element nicht erstellen!");
	} else {
		// location muss zwingend später überschrieben werden
		g_object_set (sink, "location", "/tmp/test.ogg", NULL);
	}

	// Alle Elemente zur Pipeline hinzufügen
	gst_bin_add_many (GST_BIN (ripper), source, queue, converter, encoder, muxer, sink, NULL);

	// Alle Elemente zusammenlinken
	if (!gst_element_link_many (source, queue, converter, encoder, muxer, sink, NULL)) {
		g_warning ("Konnte ripper elemente nicht linken!");
	}
	
	// Bus Callback anbinden
	bus = gst_pipeline_get_bus (GST_PIPELINE (ripper));
	gst_bus_add_watch (GST_BUS (bus), ripper_bus_callback, NULL);
	
	//g_signal_connect (bus, "progress", G_CALLBACK (on_progress_cb), NULL);
    //g_signal_connect (bus, "completion", G_CALLBACK (on_completion_cb), NULL);
    //g_signal_connect (bus, "error", G_CALLBACK (on_error_cb), NULL);
	
	gst_object_unref (bus);
}


void ripper_test (void)
{
	
	
	
}



// Callback Behandlung
gboolean ripper_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	//g_message ("GStreamer -> Got \"%s\" message from \"%s\"", GST_MESSAGE_TYPE_NAME(message), GST_ELEMENT_NAME (GST_MESSAGE_SRC (message)));
	
	if (GST_MESSAGE_TYPE (message) == GST_MESSAGE_TAG) {
		/* Musik Tags */
		//g_message ("GStreamer -> Got \"%s\" message from \"%s\"", GST_MESSAGE_TYPE_NAME(message), GST_ELEMENT_NAME (GST_MESSAGE_SRC (message)));
		//player_handle_tag_message (message);
	}
	
	if (GST_MESSAGE_SRC(message) != GST_OBJECT(ripper)) {		
		return TRUE;
	}

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gst_message_parse_error (message, &err, NULL);
			g_warning (err->message);
			g_error_free (err);
			break;
		}
		case GST_MESSAGE_STATE_CHANGED: {
			GstState oldstate, newstate, pending;
			
			gst_message_parse_state_changed(message, &oldstate, &newstate, &pending);
			//g_message ("States: (old=%i, new=%i, pending=%i)", oldstate, newstate, pending);

			if (newstate == 4) {
				g_message ("Ripper is now ripping! :)");	
			}
			if ((newstate == 2) && (oldstate == 3)) {
				g_message ("Ripper is now ready!");
			}
			if ((newstate == 3) && (oldstate == 4)) {
				g_message ("Ripper is now paused!");
			}
		}
		case GST_MESSAGE_EOS: {
			if (g_ascii_strcasecmp(gst_message_type_get_name (GST_MESSAGE_TYPE (message)), "eos") == 0) {
				
				
				g_message ("Track fertig importiert!");
				
				
				GtkListStore *store;
				TrackDetails *track;
				
				if (find_next ()) {
					store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
					gtk_tree_model_get (GTK_TREE_MODEL (store), &current, COLUMN_DETAILS, &track, -1);
					extract_track (track);
				} else {
					g_message ("Disc komplett importiert!");
				}
				
				
			}
			break;
		}
		default: {
			/* unhandled message */
			//g_debug ("Unhandled Message %i", GST_MESSAGE_TYPE (message));
			break;
		}
	}

	/* we want to be notified again the next time there is a message
	* on the bus, so returning TRUE (FALSE means we want to stop watching
	* for messages on the bus and our callback should not be called again) */
	return TRUE;
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
	GtkWidget *widget;
	
	// CD Titel
	widget = interface_get_widget ("entry_disc_title");
	gtk_entry_set_text (GTK_ENTRY (widget), g_strdup (album->title));
	
	// CD Artist
	widget = interface_get_widget ("entry_disc_artist");
	gtk_entry_set_text (GTK_ENTRY (widget), g_strdup (album->artist));
	
	// CD Genre
	widget = interface_get_widget ("entry_disc_genre");
	gtk_entry_set_text (GTK_ENTRY (widget), g_strdup (album->genre));
	
	// Release Jahr
	widget = interface_get_widget ("spinbutton_disc_year");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), (gdouble)g_date_get_year (album->release_date));
	
	// CD Nummer
	widget = interface_get_widget ("spinbutton_disc_discno");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), (gdouble)1);
	
	// Compilation
	set_compilation (album->compilation);
}



void on_button_reread_clicked (GtkWidget *widget, gpointer user_data)
{	
	GtkListStore *store;
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
	
	// Vorhandene Tracks zuerst löschen
	gtk_list_store_clear (store);
	
	
	// FIXME: hier muss zuerst Speicher freigegeben werden
	the_album = musicbrainz_lookup_cd ();
	if (the_album == NULL) {
		g_warning ("CD konnte nich eingelesen werden!");
	} else {
		g_message ("CD einlesen (title: %s | artist: %s | tracks: %d)", the_album->title,
														  the_album->artist,
														  the_album->number);

		// CD Namen anzeigen
		display_disctitle (the_album);

		// Tracks ins Grid einfüllen
		GList *tracks = g_list_first(the_album->tracks);
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
	column = gtk_tree_view_column_new_with_attributes ("Import", toggle_renderer,
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
	column = gtk_tree_view_column_new_with_attributes ("Titel",
										   title_renderer,
										   "text", COLUMN_TITLE,
										   NULL);
	gtk_tree_view_column_set_resizable (column, TRUE);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (disc_tree), column);
	
	
	// Artist
	artist_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Interpret",
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
	column = gtk_tree_view_column_new_with_attributes ("Länge",
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
		++total_extracting;
	}
	

	return FALSE;
}



void extract_track (TrackDetails *track)
{
	g_message ("<extract_track> Rippe Track: %d", track->number);
	
	gchar *filename;
	gchar *filepath;
	
	create_artist_dir (track->artist);
	create_album_dir (track->album->title, track->artist);
	
	filename = get_track_name (track->title, track->artist, track->number, ".ogg");
	filepath = get_track_path (filename, track->album->title, track->artist);
	
	//g_message ("filepath: %s", filepath);
	
	gst_element_set_state (ripper, GST_STATE_NULL);
	
	// Track von Source holen
	g_object_set (source, "track", track->number, NULL);
	
	// Dateipfad auf Sink setzen
	g_object_set (sink, "location", filepath, NULL);
	
	// Tags setzen
	GstTagList *taglist;
	taglist = gst_tag_list_new ();
	gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_TITLE, track->title, NULL);
	gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, track->artist, NULL);
	gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_ALBUM, track->album->title, NULL);
	gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_TRACK_NUMBER, track->number, NULL);
	if (track->album->release_date) {
		gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_DATE, track->album->release_date, NULL);
	}
	gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_COMMENT, "ripped by tractasono", NULL);
	gst_tag_setter_merge_tags  (GST_TAG_SETTER (encoder), taglist, GST_TAG_MERGE_REPLACE_ALL);
	gst_tag_list_free (taglist);
	
	gst_element_set_state (ripper, GST_STATE_PLAYING);
}



void extract_disc (void)
{
	GtkListStore *store;
	TrackDetails *track = NULL;
	
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
	
	// Alle zu rippende Tracks zählen
	gtk_tree_model_foreach (GTK_TREE_MODEL (store), extract_track_cb, NULL);
	
	// Mit dem ersten Track beginnen
	gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &current);
	
	
	
	gtk_tree_model_get (GTK_TREE_MODEL (store), &current, COLUMN_DETAILS, &track, -1);
	extract_track (track);
}



// Tritt ein, wenn auf dem Track TreeView eine Zeile aktiviert wird
void on_treeview_disc_row_activated (GtkTreeView *tree,
									 GtkTreePath *path,
									 GtkTreeViewColumn *column,
									 gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	TrackDetails *track;
	
	model = gtk_tree_view_get_model (tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_tree_model_get (model, &iter, COLUMN_DETAILS, &track, -1);
	
	play_track (track);
	
	display_track_state (track->number, STATE_PLAYING);
}



void play_track (TrackDetails *track)
{
	gchar *uri;
	
	// Musik abspielen
	uri = g_strdup_printf ("cdda://%d", track->number);
	player_play_uri (uri);
	
	// Muss manuell gesetzt werden
	interface_set_songinfo (track->artist, track->title);
}



static GstElement* build_encoder (void)
{
	GstElement *encoder = NULL;
	GMAudioProfile *profile;
	gchar *pipeline;
	
	const gchar *name;
	const gchar *desc;
	const gchar *pipe;
	const gchar *ext;
	
	profile = gm_audio_profile_lookup ("cdlossy");
	
	name = gm_audio_profile_get_name (profile);
	desc = gm_audio_profile_get_description (profile);
	pipe = gm_audio_profile_get_pipeline (profile);
	ext = gm_audio_profile_get_extension (profile);
	
	//g_message ("current media profile (name=%s | pipe=%s | ext=%s)", name, pipe, ext);
	
	pipeline = g_strdup_printf ("audioresample ! audioconvert ! %s", pipe);
	encoder = gst_parse_bin_from_description (pipeline, TRUE, NULL); /* TODO: return error */
	g_free(pipeline);
	
	return encoder;
}



void display_track_state (gint track, TrackState state)
{
	GtkTreeModel *model;
	GtkTreePath* path;
	GtkTreeIter iter;
	
	path = gtk_tree_path_new_from_string (g_strdup_printf ("%d", track-1));
	
	model = gtk_tree_view_get_model (disc_tree);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_STATE, state, -1);
}


static gboolean find_next (void)
{
	GtkListStore *store;
	
	store = (GtkListStore*) gtk_tree_view_get_model (disc_tree);
	
	return gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &current);;
}





// Ripper Callbacks

static void on_error_cb (GstBus *bus, GError *error, gpointer data)
{
	g_warning ("Ripper Error!");
}

static void on_completion_cb (GstBus *bus, GError *error, gpointer data)
{
	g_message ("Ripper Completed!");
}

static void on_progress_cb (GstBus *bus, GError *error, gpointer data)
{
	g_message ("Ripper progress!");
}




gboolean get_compilation (void)
{
	GtkWidget *widget = interface_get_widget ("checkbutton_disc_compilation");
	return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}


void set_compilation (gboolean active)
{
	GtkWidget *widget = interface_get_widget ("checkbutton_disc_compilation");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), active);
}


void on_entry_disc_artist_changed (GtkEditable *editable, gpointer user_data)
{
	if (!get_compilation ()) {
		// Alle Artists ändern
		gchar *text;
		GtkWidget *widget;
		GtkTreeModel *model;
		GtkTreeIter iter;
		
		widget = interface_get_widget ("entry_disc_artist");
		text = gtk_entry_get_text (GTK_ENTRY (widget));
	
		model = gtk_tree_view_get_model (disc_tree);
		if (gtk_tree_model_get_iter_first (model, &iter)) {
			do {
				gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_ARTIST, text, -1);
			} while (gtk_tree_model_iter_next (model, &iter));
		}
	}
}
