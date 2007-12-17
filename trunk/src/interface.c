/*
 *      interface.c
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

#include "interface.h"
#include "fullscreen.h"
#include "disc.h"
#include "radio.h"
#include "music.h"
#include "settings.h"
#include "lcd.h"
#include "ipod.h"


#define SOURCE_GLADE SRCDIR"/data/tractasono.glade"
#define INSTALLED_GLADE DATADIR"/tractasono/tractasono.glade"



void interface_init (int argc, char *argv[])
{
	g_message ("Interface init");
	
	// GTK initialisieren
	g_message ("\tGTK init");
	gtk_init(&argc, &argv);
	
	// Glade initialisieren
	g_message ("\tGlade init");
	glade_init();
	
	glade = NULL;
	mainwindow = NULL;
	vbox_placeholder = NULL;
	keyboard = NULL;
	vbox_tractasono = NULL;
	actual_entry = NULL;
	
	module.music = NULL;
	module.radio = NULL;
	module.ipod = NULL;
	module.import = NULL;
	module.settings = NULL;
	module.fullscreen = NULL;
	module.disc = NULL;
	module.previous = NULL;
	
	// Das Interface laden
	GString* buildfile = g_string_new(g_get_current_dir());
	buildfile = g_string_append(buildfile, "/data/tractasono.glade");
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		buildfile = g_string_assign(buildfile, INSTALLED_GLADE);
	}
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		g_warning ("Die Glade Datei konnte nicht geladen werden!");
		exit (0);
	}
	glade = glade_xml_new(buildfile->str, NULL, NULL);
	if (glade == NULL) {
		g_error ("Fehler beim Laden der Glade Datei!");
		exit (0);
	}
	
	// Verbinde die Signale automatisch mit dem Interface
	glade_xml_signal_autoconnect(glade);
	
	// Hauptfenster holen
	mainwindow = glade_xml_get_widget(glade, "window_main");
	if (mainwindow == NULL) {
		g_print("Fehler: Konnte window_main nicht holen!\n");
	}

	// Placeholder holen
	vbox_placeholder = glade_xml_get_widget(glade, "vbox_placeholder");
	if (vbox_placeholder == NULL) {
		g_print("Fehler: Konnte vbox_placeholder nicht holen!\n");
	}
	
	// Tractasono Root holen
	vbox_tractasono = glade_xml_get_widget(glade, "vbox_tractasono");
	if (vbox_tractasono == NULL) {
		g_print("Fehler: Konnte vbox_tractasono nicht holen!\n");
	}

	// Die einzelnen Windows laden und referenzieren
	module.music = g_object_ref(glade_xml_get_widget(glade, "modul_music"));
	module.disc = g_object_ref(glade_xml_get_widget(glade, "vbox_disc"));
	module.ipod = g_object_ref(glade_xml_get_widget(glade, "ipodmodul"));
	module.settings = g_object_ref(glade_xml_get_widget(glade, "vbox_settings"));
	module.radio = g_object_ref(glade_xml_get_widget(glade, "radiomodul"));
	module.fullscreen = g_object_ref(glade_xml_get_widget(glade, "eventbox_fullscreen"));

	// Keyboard laden
	GtkWidget *vbox_placeholder_keyboard = NULL;
	vbox_placeholder_keyboard = glade_xml_get_widget(glade, "vbox_placeholder_keyboard");
	if (vbox_placeholder_keyboard == NULL) {
		g_print("Fehler: Konnte vbox_placeholder_keyboard nicht holen!\n");
	}
	keyboard = glade_xml_get_widget(glade, "alignment_keyboard");
	if (keyboard == NULL) {
		g_print("Fehler: Konnte alignment_keyboard nicht holen!\n");
	}
	gtk_widget_reparent(keyboard, vbox_placeholder_keyboard);
	gtk_widget_hide(keyboard);
	
	// Progressbar laden
	progress = GTK_PROGRESS_BAR(glade_xml_get_widget(glade, "range_song"));
	if (progress == NULL) {
		g_print("Fehler: Konnte range_song nicht holen!\n");
	}
	
	// LCD
	GtkWidget *lcdspace;
	lcdspace = glade_xml_get_widget(glade, "lcdbox");
	if (lcdspace == NULL) {
		g_error("Konnte lcdbox nicht holen!\n");
	}
	lcd = lcd_new ();
	gtk_container_add (GTK_CONTAINER (lcdspace), GTK_WIDGET (lcd));
	gtk_widget_show (GTK_WIDGET (lcd));
	// LCD Text initial setzen
	lcd_set_title (LCD (lcd), "Willkommen beim tractasono!");
	
	// Einzelne GUI Module initialisieren
	
	// Disc Modul init
	disc_init ();
	
	// Radio Modul init
	radio_init ();
	
	// Music Modul init
	music_init ();
	
	// iPod Modul init
	ipod_init ();
	
	// Settings Modul init
	settings_init ();
	
	// Vollbild Modus init
	fullscreen_init ();
	
	// Musik von Anfang an anzeigen
	interface_show_module(module.music);
}


// Allgemeiner Destroy-Event
void on_main_window_destroy(GtkWidget *widget, gpointer user_data)
{
	// Programm beenden
	g_message ("Program shutdown!");
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

	/* Change TRUE to FALSE and the main window will be destroyed with
	* a "delete_event". */
	return FALSE;
}







// Alle Button Switches hier


// Zeige das vorherige Widget im Platzhalter an (für Fullscreen Modus)
void interface_show_previous_module (void)
{
	// Fullscreen Modus verlassen
	GtkWidget *tractasono;
	
	tractasono = glade_xml_get_widget(glade, "vbox_tractasono");
	if (tractasono == NULL) {
		g_warning ("Fehler: Konnte fullscreen widgets nicht holen!");
	}
	
	gtk_widget_hide (module.fullscreen);
	gtk_widget_show (tractasono);
}


// Event-Handler für den Musik Button
void on_button_music_clicked(GtkWidget *widget, gpointer user_data)
{
	interface_show_module(module.music);
}


// Event-Handler für den Einstellungen Button
void on_button_settings_clicked(GtkWidget *widget, gpointer user_data)
{
	interface_show_module(module.settings);
}


// Event-Handler für den CD Button
void on_button_ripping_clicked(GtkWidget *widget, gpointer user_data)
{
	interface_show_module(module.disc);
}


// Event-Handler für den Vollbild Button
void on_button_fullscreen_clicked(GtkWidget *widget, gpointer user_data)
{
	// Spezialbehandlung für den Fullscreen Modus
	
	GtkWidget *vbox_main;
	GtkWidget *tractasono;
	
	vbox_main = glade_xml_get_widget(glade, "vbox_main");
	tractasono = glade_xml_get_widget(glade, "vbox_tractasono");
	if (vbox_main == NULL || tractasono == NULL) {
		g_warning ("Fehler: Konnte fullscreen widgets nicht holen!");
	}
	
	gtk_widget_hide (tractasono);
	
	gtk_widget_ref(module.fullscreen);
	if (module.fullscreen->parent) {
		gtk_container_remove(GTK_CONTAINER(module.fullscreen->parent), module.fullscreen);
		gtk_container_add(GTK_CONTAINER(vbox_main), module.fullscreen);
	}
	

	gtk_widget_show(module.fullscreen);
	
}

// Event-Handler für den iPod Button
void on_button_ipod_clicked (GtkWidget *widget, gpointer user_data)
{
	interface_show_module(module.ipod);
}

// Event-Handler für den Internetradio Button
void on_button_internetradio_clicked(GtkWidget *widget, gpointer user_data)
{
	interface_show_module(module.radio);
}




// Räume alle Fenster im placeholder auf
void interface_clean(){
	GtkContainer *container = NULL;
	GList* children = NULL;

	container = GTK_CONTAINER(vbox_placeholder);
	children = gtk_container_get_children(container);

	gtk_widget_hide(keyboard);
	for (children = g_list_first(children); children; children = g_list_next(children)) {
		gtk_container_remove(container, children->data);
	}
}





void interface_set_song_position(gint64 position)
{
	//g_debug ("interface_set_song_position(): %s (minutes:seconds)", ns_formatted(position));

	gdouble fraction;
	gint64 duration;
	
	duration = player_get_duration ();

	// Position berechnen
	if (duration > 0) {
		// fraction mode
		fraction = (gdouble) position / duration;
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), fraction);
	} else {
		// pulse mode
		//gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progress));
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), 1);
	}
}


// Setze die Song Informationen
void interface_set_songinfo(const gchar *artist, const gchar *title, const gchar *uri)
{
	g_debug ("artist: %s, title: %s, uri: %s", artist, title, uri);
	if (title) {
		lcd_set_title (LCD (lcd), title);
	}
	if (artist) {
		lcd_set_artist (LCD (lcd), artist);
	}
	if (uri) {
		lcd_set_uri (LCD (lcd), uri);
	}
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
	
	// Modul speichern für interface_show_previous_module
	GList *list = gtk_container_get_children (GTK_CONTAINER(vbox_placeholder));    
	if (list) {
		module.previous = (GtkWidget*) list->data;
		g_list_free (list);
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


// Zeige ein Widget im Vollbild Modus an
void interface_show_module_fullscreen (GtkWidget *widget)
{
	if (widget == NULL) {
		g_print("Fehler: Konnte ein Modul nicht anzeigen!\n");
	}
	
	// Modul speichern für interface_show_previous_module
	GList *list = gtk_container_get_children (GTK_CONTAINER(vbox_placeholder));    
	if (list) {
		module.previous = (GtkWidget*) list->data;
		g_list_free (list);
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


// Handler für seeking
gboolean on_range_song_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_debug("Progress Button pressed!");
	
	gint x, y, breite, hoehe, tiefe;
	gint64 position;
	gint64 duration;
	gdouble barpos;
	
	gdk_window_get_geometry(GDK_WINDOW(event->window), &x, &y, &breite, &hoehe, &tiefe);                   
    barpos = (gdouble) event->x / breite;
    
    g_debug("\tbreite=%i, position=%f, barpos=%f", breite, event->x, barpos);
	
	duration = player_get_duration();
	
	// seekposition berechnen
	position = duration * barpos;
	
	g_debug("\tduration=%"G_GINT64_FORMAT", position=%"G_GINT64_FORMAT"", duration, position);
	
	player_seek_to_position(position);

	return FALSE;
}


gboolean on_range_song_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	//g_debug("on_range_song_motion_notify_event()\n\tx=%f", event->x);
	
	
	return TRUE;
}

// Setzt auf dem Play Button auf Play oder Pause
void interface_set_playimage(const gchar *stock_id)
{
        GtkImage *playimage = NULL;
        
        playimage = GTK_IMAGE(glade_xml_get_widget(glade, "image_play"));
        if (playimage == NULL) {
                g_print("Fehler beim play Bild holen!\n");
        } else {
                gtk_image_set_from_stock(playimage, stock_id, GTK_ICON_SIZE_BUTTON);
        }
}


void interface_set_playing (PlayerState state)
{
	GtkWidget *progress;
	progress = glade_xml_get_widget(glade, "alignment_progress");
	
	switch(state) {
		case STATE_PLAY_LOCAL:	
			interface_set_playimage("gtk-media-pause");
			gtk_widget_show (progress);
			break;
		case STATE_PLAY_STREAM:
			interface_set_playimage("gtk-media-stop");
			gtk_widget_hide (progress);
			break;
		case STATE_PLAY_NOTHING:
			interface_set_playimage("gtk-media-play");
			//gtk_widget_show (progress);
			break;
	}
}

void on_trackplay_clicked (GtkButton *button, gpointer user_data)
{	
	switch(player_get_state()) {
		case STATE_PLAY_LOCAL:	
			g_debug ("Pause wurde gedrückt");
			player_set_pause ();
			break;
		case STATE_PLAY_STREAM:
			g_debug ("Stop wurde gedrückt");
			player_set_stop ();
			break;
		case STATE_PLAY_NOTHING:
			g_debug ("Play wurde gedrückt");
			player_set_play ();
			break;
	}
}

void on_trackstopp_clicked (GtkButton *button, gpointer user_data)
{	
	g_debug ("Stop wurde gedrückt");
	player_set_stop ();
}


void on_button_next_clicked (GtkButton *button, gpointer user_data)
{
	g_debug ("Next wurde gedrückt");
	player_play_next ();
}


void on_button_prev_clicked (GtkButton *button, gpointer user_data)
{
	g_debug ("Prev wurde gedrückt");
	player_play_prev ();
}


void interface_update_controls (ControlState state)
{
	GtkWidget *prev;
	GtkWidget *next;
	
	prev = glade_xml_get_widget (glade, "button_prev");
	if (prev == NULL) {
		g_warning ("Konnte den Previous Button nicht holen!");
		return;
	}
	
	next = glade_xml_get_widget (glade, "button_next");
	if (next == NULL) {
		g_warning ("Konnte den Previous Button nicht holen!");
		return;
	}
	
	switch (state) {
		case CONTROL_STATE_FIRST: {
			g_message ("ControlState: First");
			gtk_widget_set_sensitive (prev, FALSE);
			break;
		}
		case CONTROL_STATE_MID: {
			g_message ("ControlState: Mid");
			gtk_widget_set_sensitive (prev, TRUE);
			gtk_widget_set_sensitive (next, TRUE);
			break;
		}
		case CONTROL_STATE_LAST: {
			g_message ("ControlState: Last");
			gtk_widget_set_sensitive (next, FALSE);
			break;
		}
		default: {
			g_warning ("ControlState ungültig!");
		}
	}
	
}
