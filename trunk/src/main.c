#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

// Globale Includes
#include <getopt.h>

// Lokale Includes
#include "tractasono.h"
#include "settings.h"
//#include "ipod.h"
//#include "database.h"
#include "drives.h"
#include "player.h"
#include "interface.h"


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




gboolean on_hscale_song_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_print("Slider Button pressed!\n");

	interface_set_slidermove(TRUE);
	player_start_seek(widget, event, user_data);

	return FALSE;
}

// Handler für seeking
gboolean on_hscale_song_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_print("Slider Button released!\n");

	GtkRange *range = GTK_RANGE(widget);

	player_stop_seek(GTK_WIDGET(range), user_data);

	/*gdouble pos = gtk_range_get_value(range);
	player_seek_to_position(pos);*/

	interface_set_slidermove(FALSE);

	return FALSE;
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

void on_window_fullscreen_button_press_event(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Fullscreen back gedrückt!\n");
	
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


void on_trackplay_clicked(GtkButton *button, gpointer user_data)
{	
	if (interface_get_playing()) {
		g_print("Pause wurde gedrückt\n");
		player_set_stop();
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

void on_button_read_toc_clicked(GtkButton *button, gpointer user_data)
{
	g_print("Read TOC wurde gedrückt\n");
	player_play_testfile();
}


// Programmeinstieg
int main(int argc, char *argv[])
{
	#ifdef ENABLE_NLS
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		textdomain (PACKAGE);
	#endif
	
	// Variablen initialisieren
	xml = NULL;
	mainwindow = NULL;
	vbox_placeholder = NULL;
	vbox_keyboard = NULL;
	vbox_tractasono = NULL;

	actual_entry = NULL;

	// Interface initialisieren
	interface_init(&argc, &argv);

	// Player initialisieren
	player_init(&argc, &argv);
	
	// Settings initialisieren
	settings_init();

	// Drives initialisieren
	drives_init();

	// Das Interface laden
	interface_load("tractasono.glade");

	// Programmloop starten
	gtk_main();

	return 0;
}
