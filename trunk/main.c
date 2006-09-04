// Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "ipod.h"
#include "database.h"

// Globale Variablen
GladeXML *xml;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;

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

// hallo schorsch

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

	/*GtkWidget *child = NULL;
	GList* children = NULL;
	children = gtk_container_get_children(vbox_placeholder);
	for (child = g_list_first(children); child; child = g_list_next(children)) {
		g_print("Loop!\n");
		gtk_container_remove(vbox_placeholder, child);
	}*/

	gtk_widget_reparent(notebook_music, vbox_placeholder);
	gtk_widget_show(notebook_music);
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

	gtk_widget_reparent(vbox_settings, vbox_placeholder);
	gtk_widget_show(vbox_settings);
}

// Event-Handler für den Rippen Button
void on_button_ripping_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Rippen gedrückt!\n");

	// Database Testfunktion
	database_test();
}


// Handler für Fukuswechler auf dem Settings-Reiter
void on_notebook1_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	// Funktioniert nonig
	gtk_container_set_focus_child(GTK_CONTAINER(notebook), GTK_WIDGET(notebook));
	// Keyboard ausblenden in Settings Reiter
	g_print("Reiter gewechselt, schliesse Tastatur\n");
	show_keyboard(FALSE);
}

// Event-Handler für den Vollbild Button
void on_button_fullscreen_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Vollbild gedrückt!\n");
}

// Event-Handler für den Test Button
void on_testbutton_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Testbutton wurde gedrückt!\n");
}

// Event-Handler für den Enter Button auf dem Oncreen Keyboard
void on_key_eingabe_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Enter wurde auf dem Keyboard gedrückt, Keyboard wird geschlossen!\n");
	show_keyboard(FALSE);
}

// Event-Handler für Eingabefelder (Fokus In)
gboolean on_entry_focus_in_event(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Eingabefeld hat Fokus bekommen!\n");
	show_keyboard(TRUE);

	return FALSE;
}


gboolean on_hscale_song_value_changed(GtkRange *range, gpointer user_data)
{
	gdouble pos = 0;

	pos = gtk_range_get_value (range);

	g_print("Song Slider Value: %.0f\n", pos);

	return FALSE;
}



// Programmeinstieg
int main(int argc, char *argv[])
{
	// Variablen initialisieren
	xml = NULL;
	vbox_placeholder = NULL;
	vbox_keyboard = NULL;

	// GTK und Glade initialisieren
	gtk_init(&argc, &argv);
	glade_init();

	// Das Interface laden
	xml = glade_xml_new("tractasono.glade", NULL, NULL);

	// Verbinde die Signale automatisch mit dem Interface
	glade_xml_signal_autoconnect(xml);

	// Placeholder holen
	vbox_placeholder = glade_xml_get_widget(xml, "vbox_placeholder");
	if (vbox_placeholder == NULL) {
		g_print("Fehler: Konnte vbox_placeholder nicht holen!\n");
	}

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

	// Programmloop starten
	gtk_main();

	return 0;
}
