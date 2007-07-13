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
#include "player.h"
#include "fullscreen.h"
#include "disc.h"
#include "radio.h"
#include "music.h"


#define SOURCE_GLADE SRCDIR"/data/tractasono.glade"
#define INSTALLED_GLADE DATADIR"/tractasono/tractasono.glade"


static gfloat style_color_mods[5]={0.0,-0.1,0.2,-0.2};


void interface_init (int argc, char *argv[])
{
	g_message ("Interface init");
	
	gtk_init(&argc, &argv);
	glade_init();
	
	glade = NULL;
	mainwindow = NULL;
	vbox_placeholder = NULL;
	vbox_keyboard = NULL;
	vbox_tractasono = NULL;
	actual_entry = NULL;
	
	module.music = NULL;
	module.radio = NULL;
	module.import = NULL;
	module.settings = NULL;
	module.fullscreen = NULL;
	module.disc = NULL;
	
	
	// Das Interface laden
	GString* buildfile = g_string_new(g_get_current_dir());
	buildfile = g_string_append(buildfile, "/data/tractasono.glade");
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		buildfile = g_string_assign(buildfile, INSTALLED_GLADE);
	}
	if (g_file_test(buildfile->str, G_FILE_TEST_EXISTS) == FALSE) {
		g_error("Die Glade Datei konnte nicht geladen werden!\n");
		exit(0);
	}
	glade = glade_xml_new(buildfile->str, NULL, NULL);
	
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
	module.music = g_object_ref(glade_xml_get_widget(glade, "notebook_music"));
	module.disc = g_object_ref(glade_xml_get_widget(glade, "vbox_disc"));
	module.settings = g_object_ref(glade_xml_get_widget(glade, "vbox_settings"));
	module.radio = g_object_ref(glade_xml_get_widget(glade, "radiomodul"));
	module.fullscreen = g_object_ref(glade_xml_get_widget(glade, "vbox_fullscreen"));

	// Keyboard laden
	GtkWidget *vbox_placeholder_keyboard = NULL;
	vbox_placeholder_keyboard = glade_xml_get_widget(glade, "vbox_placeholder_keyboard");
	if (vbox_placeholder_keyboard == NULL) {
		g_print("Fehler: Konnte vbox_placeholder_keyboard nicht holen!\n");
	}
	vbox_keyboard = glade_xml_get_widget(glade, "vbox_keyboard");
	if (vbox_keyboard == NULL) {
		g_print("Fehler: Konnte vbox_keyboard nicht holen!\n");
	}
	gtk_widget_reparent(vbox_keyboard, vbox_placeholder_keyboard);
	gtk_widget_hide(vbox_keyboard);
	
	// Progressbar laden
	progress = GTK_PROGRESS_BAR(glade_xml_get_widget(glade, "range_song"));
	if (progress == NULL) {
		g_print("Fehler: Konnte range_song nicht holen!\n");
	}
	
	// LCD
	GtkWidget *eventbox;
	eventbox = glade_xml_get_widget(glade, "eventbox_lcd");
	if (eventbox == NULL) {
		g_error("Konnte eventbox_lcd nicht holen!\n");
	}
	GtkStyle * style_LCD;
	GdkColor *color_LCD;
	color_LCD = interface_create_color (33686,38273,29557);
	style_LCD = interface_create_style (color_LCD, color_LCD, FALSE);
	gtk_widget_set_style (GTK_WIDGET(eventbox), style_LCD);
	
	
	// Disc Modul init
	disc_init ();
	
	// Radio Modul init
	radio_init ();
	
	// Music Modul init
	music_init ();
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


// Event-Handler für den Internetradio Button
void on_button_internetradio_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Button Internetradio wurde gedrückt!\n");

	interface_show_module(module.radio);
	
	radio_init(); 
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





void interface_set_song_position(gint64 position)
{
	g_debug("interface_set_song_position()");
	g_debug("\tposition is %s (minutes:seconds)", ns_formatted(position));

	gdouble fraction;
	gint64 duration;
	
	duration = player_get_duration();

	// Position berechnen
	if (duration > 0) {
		// fraction mode
		g_debug("\tfraction mode");
		fraction = (gdouble) position / duration;
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), fraction);
	} else {
		// pulse mode
		g_debug("\tpulse mode");
		gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progress));
	}
}


// Setze die Song Informationen
void interface_set_songinfo(const gchar *artist, const gchar *title, const gchar *uri)
{
	GtkLabel *label;
	
	label = GTK_LABEL(glade_xml_get_widget(glade, "label_song"));
	if (label == NULL) {
		g_error("Konnte label_song nicht holen!\n");
	}

	GString *info = NULL;

	info = g_string_new("<span font_desc='Dot Matrix 20'>");

	if (!artist && !title) {
		if (uri) {
			g_string_append(info, uri);	
		} else {
			g_string_append(info, "tractasono");
		}
	} else if (!artist) {
		g_string_append(info, title);
	} else if (!title) {
		g_string_append(info, artist);
	} else {
		g_string_append(info, artist);
		g_string_append(info, " - ");
		g_string_append(info, title);
	}
	
	g_string_append(info, "</span>");

	gtk_label_set_label(label, info->str);
	g_string_free(info, TRUE);
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


void interface_set_playing(gboolean isplaying)
{	
	if (isplaying) {
		interface_set_playimage("gtk-media-pause");
	} else {
		interface_set_playimage("gtk-media-play");
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


void on_trackplay_clicked(GtkButton *button, gpointer user_data)
{	
	if (player_get_playing()) {
		g_print("Pause wurde gedrückt\n");
		player_set_pause();
	} else {
		g_print("Play wurde gedrückt\n");
		player_set_play();
	}
}

void on_trackstopp_clicked(GtkButton *button, gpointer user_data)
{	
	g_print("Stop wurde gedrückt\n");
	player_set_stop();
}

/*
 * create_style()
 *
 * Ripped from grip, Copyright (c) 1998-2002 Mike Oliphant
 */
GtkStyle* interface_create_style (GdkColor *fg, GdkColor *bg, gboolean do_grade)
{
  GtkStyle *def;
  GtkStyle *sty;
  int state;

  def=gtk_widget_get_default_style();
  sty=gtk_style_copy(def);

  for(state=0;state<5;state++) {
    if(fg) sty->fg[state]=*fg;

    if(bg) sty->bg[state]=*bg;

    if(bg && do_grade) {
      sty->bg[state].red+=sty->bg[state].red*style_color_mods[state];
      sty->bg[state].green+=sty->bg[state].green*style_color_mods[state];
      sty->bg[state].blue+=sty->bg[state].blue*style_color_mods[state];
    }
  }

  return sty;
}


/*
 * create_color (r, g, b)
 *
 * Ripped from grip, Copyright (c) 1998-2002 Mike Oliphant
 */
GdkColor* interface_create_color (int red, int green, int blue)
{
  GdkColor *c;

  c=(GdkColor *)g_malloc(sizeof(GdkColor));
  c->red=red;
  c->green=green;
  c->blue=blue;

  gdk_color_alloc(gdk_colormap_get_system(),c);

  return c;
}


