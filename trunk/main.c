// Includes
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <getopt.h>
#include <gst/gst.h>

#include "settings.h"
#include "ipod.h"
#include "database.h"
#include "drives.h"

// Globale Variablen
GladeXML *xml;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;

GtkWidget *window_music;
GtkWidget *window_import;
GtkWidget *window_settings;
GtkWidget *window_fullscreen;
GtkWidget *window_disc;

GtkEntry *actual_entry;

GstElement *pipeline;


enum
{
	NR_COLUMN,
	TITLE_COLUMN,
	ARTIST_COLUMN,
	ALBUM_COLUMN,
	N_COLUMNS
};



// Allgemeiner Destroy-Event
void on_main_window_destroy(GtkWidget *widget, gpointer user_data)
{
	// Programm beenden
	g_print("Programm wird beendet!\n");
	gtk_main_quit();
}

// Delete-Event vom Windowmanager
gboolean on_main_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	/* If you return FALSE in the "delete_event" signal handler,
	* GTK will emit the "destroy" signal. Returning TRUE means
	* you don't want the window to be destroyed.
	* This is useful for popping up 'are you sure you want to quit?'
	* type dialogs. */
	g_print ("Das Programm wird durch den Windowmanager geschlossen!\n");

	/* Change TRUE to FALSE and the main window will be destroyed with
	* a "delete_event". */
	return FALSE;
}

// Zeige ein Widget im Platzhalter an
void add_to_placeholder(GtkWidget *widget)
{
	gtk_widget_ref(widget);
	if (widget->parent) {
		gtk_container_remove(GTK_CONTAINER(widget->parent), widget);
	}
	gtk_container_add(GTK_CONTAINER(vbox_placeholder), widget);
	gtk_widget_unref(widget);

	gtk_widget_show(widget);
}

// Räume alle Fenster im placeholder auf
void clean_placeholder()
{
	GtkContainer *container = NULL;
	GtkWidget *child = NULL;
	GList* children = NULL;
	//gchararray name;

	container = GTK_CONTAINER(vbox_placeholder);
	children = gtk_container_get_children(container);

	//g_print("Anzahl Children: %u\n", g_list_length(children));
	gtk_widget_hide(vbox_keyboard);
	for (children = g_list_first(children); children; children = g_list_next(children)) {
		//g_object_get(GTK_OBJECT(children->data), "name", &name, NULL);
		//g_print("Child name: %s\n", name);
		gtk_container_remove(container, children->data);
	}
}

// Setze die Song Informationen (Artist - Title   (Seconds))
void set_song_info(const gchar *artist, const gchar *title, gint seconds)
{
	GtkLabel *song = NULL;
	song = GTK_LABEL(glade_xml_get_widget(xml, "label_song"));

	if (song == NULL) {
		g_print("Fehler: Konnte label_song nicht holen!\n");
	}

	GString *newsong = NULL;

	newsong = g_string_new("<span size=\"xx-large\" weight=\"heavy\">");
	g_string_append(newsong, artist);
	g_string_append(newsong, " - ");
	g_string_append(newsong, title);
	//g_string_append(newsong, "   ()");
	g_string_append(newsong, "</span>");

	gtk_label_set_label(song, newsong->str);
}

// Zeige das Oncreen Keyboard an
void show_keyboard(gboolean show)
{
	if (show) {
		// Keyboard anzeigen
		g_print("Keyboard wird angezeigt!\n");

		gboolean visible;
		g_object_get(vbox_keyboard, "visible", &visible, NULL);
		if (!visible) {	
			gtk_widget_show(vbox_keyboard);
		}
		
	} else {
		// Keyboard ausblenden
		g_print("Keyboard wird ausgeblendet!\n");
		gtk_widget_hide(vbox_keyboard);
	}
}

// Event-Handler für den Musik Button
void on_button_music_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Musik gedrückt!\n");

	// Einstellungen Window holen
	GtkWidget *notebook_music = NULL;

	notebook_music = glade_xml_get_widget(xml, "notebook_music");
	if (notebook_music == NULL) {
		g_print("Fehler: Konnte notebook_music nicht holen!\n");
	}

	clean_placeholder();
	add_to_placeholder(notebook_music);
}

// Event-Handler für den Einstellungen Button
void on_button_settings_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Einstellungen gedrückt!\n");

	// Einstellungen Window holen
	GtkWidget *vbox_settings = NULL;

	vbox_settings = glade_xml_get_widget(xml, "vbox_settings");
	if (vbox_settings == NULL) {
		g_print("Fehler: Konnte vbox_settings nicht holen!\n");
	}

	clean_placeholder();
	add_to_placeholder(vbox_settings);
}

// Event-Handler für den CD Button
void on_button_ripping_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("CD gedrückt!\n");

	// CD Window holen
	GtkWidget *notebook_cd = NULL;

	notebook_cd = glade_xml_get_widget(xml, "notebook_cd");
	if (notebook_cd == NULL) {
		g_print("Fehler: Konnte notebook_cd nicht holen!\n");
	}

	clean_placeholder();
	add_to_placeholder(notebook_cd);

	// Database Testfunktion
	//database_test();
}


// Handler für Fukuswechler auf dem Settings-Reiter
void on_notebook1_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	gchararray name;
	g_object_get(notebook, "name", &name, NULL);
	g_print("Page name: %s\n", name);

	g_print("Reiter gewechselt, schliesse Tastatur\n");
	show_keyboard(FALSE);
}

// Event-Handler für den Vollbild Button
void on_button_fullscreen_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Vollbild gedrückt!\n");

	clean_placeholder();
}

// Event-Handler für den Test Button
void on_testbutton_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Testbutton wurde gedrückt!\n");
}

gboolean on_entry_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_print("Eingabefeld hat Fokus bekommen!\n");

	actual_entry = GTK_ENTRY(widget);
	show_keyboard(TRUE);

	return(FALSE);
}

void add_text_to_entry(const GString *text)
{
	const gchar *origtext = NULL;
	GString *newtext = NULL;

	origtext = gtk_entry_get_text(actual_entry);
	newtext = g_string_new(origtext);
	g_string_append(newtext, text->str);
	
	gtk_entry_set_text(actual_entry, newtext->str);

	gtk_widget_grab_focus(GTK_WIDGET(actual_entry));
}

// Tastatur Spezial Tastenhandler
void on_specialkey_clicked(GtkButton *button, gpointer user_data)
{
	gchararray name;

	g_object_get(G_OBJECT(button), "name", &name, NULL);

	
	if (g_strcasecmp (name, "key_enter") == 0) {
		g_print("space\n");
		show_keyboard(FALSE);
		return;
	}

	if (g_strcasecmp (name, "key_esc") == 0) {
		g_print("ESC\n");
		show_keyboard(FALSE);
		return;
	}

	if (g_strcasecmp (name, "key_back") == 0) {
		g_print("Back\n");
		GString *temptext = NULL;
		temptext = g_string_new(gtk_entry_get_text(actual_entry));
		g_string_truncate(temptext, temptext->len -1);
		add_text_to_entry(temptext);
		return;
	}

	if (g_strcasecmp (name, "key_caps") == 0) {
		g_print("caps\n");
		return;
	}

	if (g_strcasecmp (name, "key_lshift") == 0) {
		g_print("shift\n");
		return;
	}

	if (g_strcasecmp (name, "key_rshift") == 0) {
		g_print("shift\n");
		return;
	}

	if (g_strcasecmp (name, "key_space") == 0) {
		g_print("space\n");
		GString *text = g_string_new(" ");
		add_text_to_entry(text);
		return;
	}
}

// Tastatur Tastenhandler
void on_key_clicked(GtkButton *button, gpointer user_data)
{
	GString tastenlabel;
	
	g_object_get(G_OBJECT(button), "label", &tastenlabel, NULL);
	add_text_to_entry(&tastenlabel);
}

// Song Slider Event Handler
gboolean on_hscale_song_value_changed(GtkRange *range, gpointer user_data)
{
	gdouble pos = 0;

	pos = gtk_range_get_value (range);

	g_print("Song Slider Value: %.0f\n", pos);

	return FALSE;
}

static gboolean my_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	g_print ("GStreamer: Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("\tError: %s\n", err->message);
			g_error_free (err);
			g_free (debug);
			break;
		}
		case GST_MESSAGE_EOS: {
			/* end-of-stream */
			g_print ("\tEnd Of Stream\n");
			break;
		}
		default: {
			/* unhandled message */
			g_print ("\tUnhandled Message %i\n", GST_MESSAGE_TYPE (message));
			break;
		}
	}

	/* we want to be notified again the next time there is a message
	* on the bus, so returning TRUE (FALSE means we want to stop watching
	* for messages on the bus and our callback should not be called again) */
	return TRUE;
}


void on_trackplay_clicked(GtkButton *button, gpointer user_data)
{
	g_print("Track play\n");
	
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);

}

void on_trackstopp_clicked(GtkButton *button, gpointer user_data)
{
	g_print("Track stopp\n");

	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PAUSED);

}

void on_button_read_toc_clicked(GtkButton *button, gpointer user_data)
{
	g_print("Read TOC\n");

	/*
	GtkTreeView *tree = NULL;
	GtkTreeIter   iter;
	GtkTreeStore *store = NULL;

	tree = (GtkTreeView*) glade_xml_get_widget(xml, "treeview_actual_cd");
	if (tree == NULL) {
		g_print("Fehler: Konnte treeview_actual_cd nicht holen!\n");
	}

	store = gtk_tree_store_new (	N_COLUMNS,
							G_TYPE_INT,
							G_TYPE_STRING,
							G_TYPE_STRING,
							G_TYPE_STRING);


	gtk_tree_view_set_model(tree, store);

	gtk_tree_store_append (store, &iter, NULL);

	gtk_tree_store_set (store, &iter,	NR_COLUMN, 1,
								TITLE_COLUMN, "The Principle of Reason",
								ARTIST_COLUMN, "Martin Heidegger",
								ALBUM_COLUMN, "Test", -1);
	*/


	
	
	GstElement *source, *filter, *sink;
	GstBus *bus;

	pipeline = gst_pipeline_new ("my-pipeline");
	
	source = gst_element_factory_make ("filesrc", "source");
	g_object_set (source, "location", "test.mp3", NULL);

	filter = gst_element_factory_make ("mad", "filter");
	sink = gst_element_factory_make ("alsasink", "sink");


	gst_bin_add_many (GST_BIN (pipeline), source, filter, sink, NULL);

	/* adds a watch for new message on our pipeline's message bus to
	* the default GLib main context, which is the main context that our
	* GLib main loop is attached to below */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, my_bus_callback, NULL);
	gst_object_unref (bus);


	if (!gst_element_link_many (source, filter, sink, NULL)) {
		g_warning ("Failed to link elements!");
	}

}

// GStreamer initialisieren
void gstreamer_init()
{
	g_print("GStreamer initialisieren\n");

	const gchar *nano_str;
  	guint major, minor, micro, nano;

	gst_version (&major, &minor, &micro, &nano);

	if (nano == 1)
		nano_str = "(CVS)";
	else if (nano == 2)
		nano_str = "(Prerelease)";
	else
		nano_str = "";

	g_print("This program is linked against GStreamer %d.%d.%d %s\n", major, minor, micro, nano_str);
}


// Programmeinstieg
int main(int argc, char *argv[])
{
	// Variablen initialisieren
	xml = NULL;
	mainwindow = NULL;
	vbox_placeholder = NULL;
	vbox_keyboard = NULL;

	window_music = NULL;
	window_import = NULL;
	window_settings = NULL;
	window_fullscreen = NULL;
	window_disc = NULL;

	actual_entry = NULL;

	// GTK und Glade initialisieren
	gtk_init(&argc, &argv);
	glade_init();

	// GStreamer initialisieren
	gst_init (&argc, &argv);
	gstreamer_init();

	// Drives initialisieren
	drives_init();

	// Settings initialisieren
	settings_init();

	

	// Das Interface laden
	xml = glade_xml_new("tractasono.glade", NULL, NULL);

	// Verbinde die Signale automatisch mit dem Interface
	glade_xml_signal_autoconnect(xml);

	// Hauptfenster holen
	mainwindow = glade_xml_get_widget(xml, "window_main");
	if (mainwindow == NULL) {
		g_print("Fehler: Konnte window_main nicht holen!\n");
	}

	// Placeholder holen
	vbox_placeholder = glade_xml_get_widget(xml, "vbox_placeholder");
	if (vbox_placeholder == NULL) {
		g_print("Fehler: Konnte vbox_placeholder nicht holen!\n");
	}

	// Die einzelnen Windows laden und referenzieren
	window_music = g_object_ref(glade_xml_get_widget(xml, "notebook_music"));
	window_disc = g_object_ref(glade_xml_get_widget(xml, "notebook_cd"));
	window_settings = g_object_ref(glade_xml_get_widget(xml, "vbox_settings"));
	//window_fullscreen = g_object_ref(glade_xml_get_widget(xml, ""));

	// Keyboard laden
	GtkWidget *vbox_placeholder_keyboard = NULL;
	vbox_placeholder_keyboard = glade_xml_get_widget(xml, "vbox_placeholder_keyboard");
	if (vbox_placeholder_keyboard == NULL) {
		g_print("Fehler: Konnte vbox_placeholder_keyboard nicht holen!\n");
	}
	vbox_keyboard = glade_xml_get_widget(xml, "vbox_keyboard");
	if (vbox_keyboard == NULL) {
		g_print("Fehler: Konnte vbox_keyboard nicht holen!\n");
	}
	gtk_widget_reparent(vbox_keyboard, vbox_placeholder_keyboard);
	gtk_widget_hide(vbox_keyboard);

	// Song Range setzen (Test)
	GtkRange *range = NULL;
	range = (GtkRange *)glade_xml_get_widget(xml, "hscale_song");
	gtk_range_set_range(range, 0, 168);
	gtk_range_set_value(range, 0);

	//set_song_info("Helloween", "The Dark Ride", 0);

	// Programmloop starten
	gtk_main();

	return 0;
}
