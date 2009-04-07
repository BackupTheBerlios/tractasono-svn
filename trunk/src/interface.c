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
#include "database.h"


#define SOURCE_GLADE SRCDIR"/data/tractasono.ui"
#define INSTALLED_GLADE DATADIR"/tractasono/tractasono.ui"
#define INSTALLED_ICON  ICON_DIR"/tractasono-icon.png"



void interface_init (int argc, char *argv[])
{
	g_message ("Interface init");
	
	// GTK initialisieren
	g_message ("\tGTK init");
	gtk_init(&argc, &argv);
	
	ui = NULL;
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
	buildfile = g_string_append(buildfile, "/data/tractasono.ui");
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		buildfile = g_string_assign(buildfile, INSTALLED_GLADE);
	}
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		g_warning ("Die Glade Datei konnte nicht geladen werden!");
		exit (0);
	}
	
	GError* error = NULL;
	ui = gtk_builder_new();
	if (!gtk_builder_add_from_file (ui, buildfile->str, &error)) {
		g_error ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	
	// Verbinde die Signale automatisch mit dem Interface
	gtk_builder_connect_signals (ui, NULL);
	
	// Hauptfenster holen
	mainwindow = interface_get_widget ("window_main");
	
	// Icon setzen
	const gchar* icon = INSTALLED_ICON;
	if (g_file_test(icon, G_FILE_TEST_EXISTS) == TRUE) {
		gtk_window_set_icon_from_file (GTK_WINDOW (mainwindow), icon, NULL);
	}

	// Placeholder holen
	vbox_placeholder = interface_get_widget ("vbox_placeholder");
	
	// Tractasono Root holen
	vbox_tractasono = interface_get_widget ("vbox_tractasono");

	// Die einzelnen Windows laden und referenzieren
	module.music = g_object_ref (interface_get_widget ("modul_music"));
	module.disc = g_object_ref (interface_get_widget ("vbox_disc"));
	module.ipod = g_object_ref (interface_get_widget ("ipodmodul"));
	module.settings = g_object_ref (interface_get_widget ("vbox_settings"));
	module.radio = g_object_ref (interface_get_widget ("radiomodul"));
	module.fullscreen = g_object_ref (interface_get_widget ("eventbox_fullscreen"));

	// Keyboard laden
	GtkWidget *vbox_placeholder_keyboard = NULL;
	vbox_placeholder_keyboard = interface_get_widget ("vbox_placeholder_keyboard");
	keyboard = interface_get_widget ("alignment_keyboard");
	gtk_widget_reparent(keyboard, vbox_placeholder_keyboard);
	gtk_widget_hide(keyboard);
	
	// Progressbar laden
	progress = GTK_PROGRESS_BAR( interface_get_widget ("range_song"));
	
	// LCD
	GtkWidget *lcdspace;
	lcdspace = interface_get_widget ("lcdbox");
	lcd = lcd_new ();
	gtk_container_add (GTK_CONTAINER (lcdspace), GTK_WIDGET (lcd));
	
	// LCD Text initial setzen
	gchar *title = db_settings_get_text ("LCD", "StartText");
	lcd_set_title (LCD (lcd), title);
	
	// Widget anzeigen
	gtk_widget_show (GTK_WIDGET (lcd));
	
	// Einzelne GUI Module initialisieren
	disc_init ();
	radio_init ();
	music_init ();
	ipod_init ();
	settings_init ();
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
	
	tractasono = interface_get_widget ("vbox_tractasono");
	
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
	
	vbox_main = interface_get_widget ("vbox_main");
	tractasono = interface_get_widget ("vbox_tractasono");
	
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
void interface_set_songinfo (const gchar *artist, const gchar *title)
{
	//g_message ("interface_set_songinfo: %s, title: %s", artist, title);
	if (title) {
		lcd_set_title (LCD (lcd), title);
	}
	if (artist) {
		lcd_set_artist (LCD (lcd), artist);
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
	g_debug ("Progress Button pressed!");
	
	gint x, y, breite, hoehe, tiefe;
	gint64 position;
	gint64 duration;
	gdouble barpos;
	
	gdk_window_get_geometry (GDK_WINDOW(event->window), &x, &y, &breite, &hoehe, &tiefe);                   
    barpos = (gdouble) event->x / breite;
    
    //g_debug("\tbreite=%i, position=%f, barpos=%f", breite, event->x, barpos);
	
	duration = player_get_duration();
	
	// seekposition berechnen
	position = duration * barpos;
	
	// Slider Position temporär setzen
	interface_set_song_position (position);
	
	//g_debug("\tduration=%"G_GINT64_FORMAT", position=%"G_GINT64_FORMAT"", duration, position);
	
	player_seek_to_position (position);

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

	playimage = GTK_IMAGE( interface_get_widget ("image_play"));
	if (playimage != NULL) {
		gtk_image_set_from_stock(playimage, stock_id, GTK_ICON_SIZE_BUTTON);
	}
}


void interface_set_playing (PlayerState state)
{
	GtkWidget *progress_area;
	progress_area = interface_get_widget ("alignment_progress");
	
	switch(state) {
		case STATE_PLAY_LOCAL:	
			interface_set_playimage("gtk-media-pause");
			gtk_widget_show (progress_area);
			break;
		case STATE_PLAY_STREAM:
			interface_set_playimage("gtk-media-stop");
			gtk_widget_hide (progress_area);
			break;
		case STATE_PLAY_NOTHING:
			interface_set_playimage("gtk-media-play");
			interface_set_songinfo ("", "");
			gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), 0);
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


void interface_update_controls (PlayList *playlist)
{
	g_return_if_fail (playlist != NULL);
	
	GtkWidget *prev;
	GtkWidget *next;
	
	prev = interface_get_widget ("button_prev");
	next = interface_get_widget ("button_next");
	
	gtk_widget_set_sensitive (prev, playlist_has_prev (playlist));
	gtk_widget_set_sensitive (next, playlist_has_next (playlist));
}


GtkWidget* interface_get_widget (gchar *name)
{
	GtkWidget *widget;
	
	widget = GTK_WIDGET (gtk_builder_get_object (ui, name));
	if (widget == NULL) {
		g_error ("Das Widget '%s' konnte nicht geholt werden!", name);
	}
	
	return widget;
}
