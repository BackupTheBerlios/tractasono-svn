#include "interface.h"
#include "player.h"
//#include "radio.h"
#include "tractasono.h"
#include "fullscreen.h"

GladeXML  *xml;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;
GtkWidget *vbox_tractasono;

GtkRange *range = NULL;
GtkAdjustment *adjustment;

GtkEntry *actual_entry;

gboolean slidermove;
gboolean playing;


#define SOURCE_GLADE SRCDIR"/data/tractasono.glade"
#define INSTALLED_GLADE DATADIR"/tractasono/tractasono.glade"



void interface_init(int *argc, char ***argv)
{
	g_print("Interface initialisieren\n");
	gtk_init(argc, argv);
	glade_init();
	
	module.music = NULL;
	module.radio = NULL;
	module.import = NULL;
	module.settings = NULL;
	module.fullscreen = NULL;
	module.disc = NULL;

	slidermove = FALSE;
	playing = FALSE;
	adjustment = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.00, 100.0,
												   0.1, 1.0, 1.0));
}


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







// Alle Button Switches hier


// Event-Handler für den Internetradio Button
void on_button_internetradio_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Button Internetradio wurde gedrückt!\n");

	interface_show_module(module.radio);
	
	//init_radio_modul(); 
}




// Räume alle Fenster im placeholder auf
void interface_clean(){
	GtkContainer *container = NULL;
	GList* children = NULL;

	container = GTK_CONTAINER(vbox_placeholder);
	children = gtk_container_get_children(container);

	gtk_widget_hide(vbox_keyboard);
	for (children = g_list_first(children); children; children = g_list_next(children)) {
		gtk_container_remove(container, children->data);
	}
}





void interface_set_song_position(gdouble position)
{	
	//g_print("interface_set_song_position: %.0f\n", position);
	gtk_range_set_value(range, position);
}

void interface_set_song_duration(gdouble duration)
{		
	//g_print("interface_set_song_duration: %.0f\n", duration);
	gtk_range_set_range(range, 0, duration);
}


void interface_load(const gchar *gladefile)
{
	// Das Interface laden
	// HACK (sollte mal angeschaut werden)
	GString* buildfile = g_string_new(g_get_current_dir());;
	buildfile = g_string_append(buildfile, "/data/tractasono.glade");
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		buildfile = g_string_assign(buildfile, INSTALLED_GLADE);
	}
	xml = glade_xml_new(buildfile->str, NULL, NULL);
	
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
	
	// Tractasono Root holen
	vbox_tractasono = glade_xml_get_widget(xml, "vbox_tractasono");
	if (vbox_tractasono == NULL) {
		g_print("Fehler: Konnte vbox_tractasono nicht holen!\n");
	}

	// Die einzelnen Windows laden und referenzieren
	module.music = g_object_ref(glade_xml_get_widget(xml, "notebook_music"));
	module.disc = g_object_ref(glade_xml_get_widget(xml, "notebook_cd"));
	module.settings = g_object_ref(glade_xml_get_widget(xml, "vbox_settings"));
	module.radio = g_object_ref(glade_xml_get_widget(xml, "table_radio"));
	module.fullscreen = g_object_ref(glade_xml_get_widget(xml, "vbox_fullscreen"));

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
	
	// Range laden
	range = (GtkRange *)glade_xml_get_widget(xml, "hscale_song");
	
	if (range == NULL) {
		g_print("Fehler: Konnte hscale_song nicht holen!\n");
	}
}

// Setze die Song Informationen
void interface_set_songinfo(const gchar *artist,
							const gchar *title,
							gdouble seconds)
{
	GtkLabel *song = NULL;
	
	song = GTK_LABEL(glade_xml_get_widget(xml, "label_song"));

	if (song == NULL) {
		g_print("Fehler: Konnte label_song nicht holen!\n");
	}

	GString *newsong = NULL;

	newsong = g_string_new("<span size=\"xx-large\" weight=\"heavy\">");
	if (g_ascii_strcasecmp(artist, "") == 0 && g_ascii_strcasecmp(title, "") == 0) {
		g_string_append(newsong, "Keine Song Informationen vorhanden!");
	} else if(g_ascii_strcasecmp(artist, "") != 0 && g_ascii_strcasecmp(title, "") == 0) {
		g_string_append(newsong, artist);
	} else if(g_ascii_strcasecmp(artist, "") == 0 && g_ascii_strcasecmp(title, "") != 0) {
		g_string_append(newsong, title);
	} else if(g_ascii_strcasecmp(artist, "") != 0 && g_ascii_strcasecmp(title, "") != 0) {
		g_string_append(newsong, artist);
		g_string_append(newsong, " - ");
		g_string_append(newsong, title);
	}
	g_string_append(newsong, "</span>");

	gtk_label_set_label(song, newsong->str);
	
	interface_set_song_duration(0);
	interface_set_song_position(0);
}

void interface_set_slidermove(gboolean move)
{
	slidermove = move;
}

gboolean interface_get_slidermove()
{
	return slidermove;
}

void interface_set_playimage(const gchar *stock_id)
{
	GtkImage *playimage = NULL;
	
	playimage = GTK_IMAGE(glade_xml_get_widget(xml, "image_play"));
	
	if (playimage == NULL) {
		g_print("Fehler beim play Bild holen!\n");
	} else {
		gtk_image_set_from_stock(playimage,
								 stock_id,
								 GTK_ICON_SIZE_BUTTON);
	}
}

void interface_set_playing(gboolean isplaying)
{
	playing = isplaying;
	
	if (playing) {
		interface_set_playimage("gtk-media-pause");
	} else {
		interface_set_playimage("gtk-media-play");
	}
}

gboolean interface_get_playing()
{
	return playing;
}


// Räume alle Fenster auf
void interface_clean_all()
{
	GtkContainer *root = NULL;
	GList* children = NULL;

	root = GTK_CONTAINER(vbox_tractasono);
	children = gtk_container_get_children(root);

	for (children = g_list_first(children); children; children = g_list_next(children)) {
		gtk_container_remove(root, children->data);
	}
}


// Zeige ein Widget im Platzhalter an
void interface_show_module(GtkWidget *widget)
{
	if (widget == NULL) {
		g_print("Fehler: Konnte ein Modul nicht anzeigen!\n");
	}
	
	interface_clean();
	
	gtk_widget_ref(widget);
	if (widget->parent) {
		gtk_container_remove(GTK_CONTAINER(widget->parent), widget);
	}
	gtk_container_add(GTK_CONTAINER(vbox_placeholder), widget);
	gtk_widget_unref(widget);

	gtk_widget_show(widget);
}



// Event-Handler für den Musik Button
void on_button_music_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Musik gedrückt!\n");

	interface_show_module(module.music);
}


// Event-Handler für den Einstellungen Button
void on_button_settings_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Einstellungen gedrückt!\n");

	interface_show_module(module.settings);
}


// Event-Handler für den CD Button
void on_button_ripping_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("CD gedrückt!\n");

	interface_show_module(module.disc);

	// Database Testfunktion
	//database_test();
}


// Event-Handler für den Vollbild Button
void on_button_fullscreen_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Vollbild gedrückt!\n");

	//interface_clean_all();
	//root = GTK_CONTAINER(vbox_tractasono);
	
	interface_show_module(module.fullscreen);
	fullscreen_init();
}





