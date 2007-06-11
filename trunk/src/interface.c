#include "interface.h"
#include "player.h"
#include "radio.h"

GladeXML  *xml;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;
GtkWidget *vbox_tractasono;

GtkWidget *window_music;
GtkWidget *window_import;
GtkWidget *window_settings;
GtkWidget *window_fullscreen;
GtkWidget *window_disc;

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

	slidermove = FALSE;
	playing = FALSE;
	adjustment = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.00, 100.0,
												   0.1, 1.0, 1.0));
}



// Alle Button Switches hier


// Event-Handler für den Internetradio Button
void on_button_internetradio_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Button Internetradio wurde gedrückt!\n");
	
	// Modul holen
	GtkWidget *radio_modul = NULL;

	//radio_modul = glade_xml_get_widget(xml, "radio_modul");
	radio_modul = glade_xml_get_widget(xml, "table_radio");
	if (radio_modul == NULL) {
		g_print("Fehler: Konnte radio_modul nicht holen!\n");
	}

	interface_clean();
	interface_add_modul(radio_modul);
	
	init_radio_modul(); 
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


// Zeige ein Modul im Platzhalter an
void interface_add_modul(GtkWidget *widget)
{
	gtk_widget_ref(widget);
	if (widget->parent) {
		gtk_container_remove(GTK_CONTAINER(widget->parent), widget);
	}
	gtk_container_add(GTK_CONTAINER(vbox_placeholder), widget);
	gtk_widget_unref(widget);

	gtk_widget_show(widget);
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







