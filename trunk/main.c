// Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "ipod.h"

// Globale Variablen
GladeXML *xml;
GladeXML *xmlSettings;

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

// Event-Handler für den Musik Button
void on_button_music_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Musik gedrückt!\n");
}

// Event-Handler für den Einstellungen Button
void on_button_settings_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Einstellungen gedrückt!\n");

	if (xmlSettings == NULL) {
		// Einstellungen Window holen
		xmlSettings = glade_xml_new("tractasono.glade", "window_settings", NULL);
		if (xmlSettings == NULL) {
			g_print("Fehler: Konnte Einstellungen Window nicht holen!\n");
		}

		GtkWidget *vbox5 = NULL;
		vbox5 = glade_xml_get_widget(xmlSettings, "vbox5");
		if (vbox5 == NULL) {
			g_print("Fehler: Konnte vbox5 nicht holen!\n");
		}

		GtkWidget *vbox3 = NULL;
		vbox3 = glade_xml_get_widget(xml, "vbox3");
		if (vbox3 == NULL) {
			g_print("Fehler: Konnte vbox3 nicht holen!\n");
		}

		gtk_widget_reparent (vbox5, vbox3);
		gtk_widget_show (vbox5);
	} 
}

// Event-Handler für den Rippen Button
void on_button_ripping_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Rippen gedrückt!\n");
}

// Event-Handler für den Vollbild Button
void on_button_fullscreen_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Vollbild gedrückt!\n");
}


// Programmeinstieg
int main(int argc, char *argv[])
{
	// Variablen initialisieren
	xml = NULL;
	xmlSettings = NULL;

	// GTK und Glade initialisieren
	gtk_init(&argc, &argv);
	glade_init();

	/* Das Interface laden */
	xml = glade_xml_new("tractasono.glade", "main_window", NULL);

	/* Verbinde die Signale automatisch mit dem Interface */
	glade_xml_signal_autoconnect(xml);

	/* Programmloop starten */
	gtk_main();

	return 0;
}
