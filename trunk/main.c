// Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "ipod.h"

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



	// Musik Window holen
	//GtkWidget *window = NULL;
	//window = glade_xml_get_widget(xml, "window_music");

	//gtk_destroy_widget("");


}

// Event-Handler für den Einstellungen Button
void on_button_settings_clicked(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Einstellungen gedrückt!\n");


	// Einstellungen Window holen
	//GtkWidget *settings = NULL;
	//settings = glade_xml_get_widget(xml, "window_music");

	//if (settings == NULL) {
	//	g_print("Fehler: Konnte Einstellungen Window nicht holen!\n");
	//}

	//gtk_destroy_widget("");

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
	// Lokale Variablen
	GladeXML *xml;
	//GtkWidget *window;

	// GTK und Glade initialisieren
	gtk_init(&argc, &argv);
	glade_init();

	/* Das Interface laden */
	xml = glade_xml_new("tractasono.glade", "main_window", NULL);

	/* Verbinde die Signale automatisch mit dem Interface */
	glade_xml_signal_autoconnect(xml);


	// Musik Window holen
	//GtkBoxChild *vbox3 = NULL;
	//GtkWidget *vbox3 = NULL;
	//GtkWidget *placeholder = NULL;
	//GtkWidget *musicwindow = NULL;
	//vbox3 = (GtkBoxChild *)glade_xml_get_widget(xml, "vbox3");
	//placeholder = glade_xml_get_widget(xml, "label_placeholder");
	//musicwindow = glade_xml_get_widget(xml, "window_music");

	//gtk_widget_destroy(placeholder);

	// Musik Window hinzufügen
	//gtk_container_add (GTK_CONTAINER (vbox3, musicwindow);
	//gtk_box_pack_end_defaults(GTK_BOX (vbox3), musicwindow);

	/* The final step is to display this newly created widget. */
	//gtk_widget_show (musicwindow);
	

	// Hauptwindow holen
	//window = glade_xml_get_widget(xml, "main_window");

	/* Here we connect the "destroy" event to a signal handler.
	* This event occurs when we call gtk_widget_destroy() on the window,
	* or if we return FALSE in the "delete_event" callback. */
	//g_signal_connect(G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

	/* When the window is given the "delete_event" signal (this is given
	* by the window manager, usually by the "close" option, or on the
	* titlebar), we ask it to call the delete_event () function
	* as defined above. The data passed to the callback
	* function is NULL and is ignored in the callback function. */
	//g_signal_connect(G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);




	// Einstellungen Window holen
	GtkWidget *settings = NULL;
	settings = glade_xml_get_widget(xml, "window_music");

	if (settings == NULL) {
		g_print("Fehler: Konnte Einstellungen Window nicht holen!\n");
	}




	/* Programmloop starten */
	gtk_main();

	return 0;
}
