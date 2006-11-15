#include "interface.h"

GladeXML *xml;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;

GtkWidget *window_music;
GtkWidget *window_import;
GtkWidget *window_settings;
GtkWidget *window_fullscreen;
GtkWidget *window_disc;

GtkRange *range = NULL;

GtkEntry *actual_entry;

gboolean slidermove;



void interface_init(int *argc, char ***argv)
{
	g_print("Interface initialisieren\n");
	gtk_init(argc, argv);
	glade_init();

	slidermove = FALSE;
}

void interface_set_song_position(gdouble position)
{	
	g_print("interface_set_song_position: %f\n", position);
	gtk_range_set_value(range, position);
}

void interface_set_song_duration(gdouble duration)
{		
	g_print("interface_set_song_duration: %f\n", duration);
	gtk_range_set_range(range, 0, duration);
}


void interface_load(const gchar *gladefile)
{
	// Das Interface laden
	if (gladefile) {
		xml = glade_xml_new(gladefile, NULL, NULL);
	} else {
		xml = glade_xml_new("tractasono.glade", NULL, NULL);
	}
	
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
	GtkRange *range = NULL;
	
	song = GTK_LABEL(glade_xml_get_widget(xml, "label_song"));

	if (song == NULL) {
		g_print("Fehler: Konnte label_song nicht holen!\n");
	}

	GString *newsong = NULL;

	newsong = g_string_new("<span size=\"xx-large\" weight=\"heavy\">");
	g_string_append(newsong, "Franz Ferdinand");
	g_string_append(newsong, " - ");
	g_string_append(newsong, title);
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
