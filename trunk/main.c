#include <gtk/gtk.h>
#include <glade/glade.h>

// Ein simpler Kommentar

// Callback Funktionen
void some_signal_handler_func(GtkWidget *widget, gpointer user_data) {
  /* do something useful here */
}


void on_button_settings_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Einstellungen gedrückt!\n");
}

int main(int argc, char *argv[])
{
    GladeXML *xml;

    gtk_init(&argc, &argv);

    /* load the interface */
    xml = glade_xml_new("tractasono.glade", NULL, NULL);

    /* connect the signals in the interface */
    glade_xml_signal_autoconnect(xml);

    /* start the event loop */
    gtk_main();

    return 0;
}
