#include <gtk/gtk.h>
#include <glade/glade.h>

#include "radio.h"
#include "player.h"
#include "tractasono.h"

void on_button_radio_stream_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Radio Stream...\n");
	
	GtkWidget *urlinput;
	
	urlinput = glade_xml_get_widget(xml, "entry_radio_url");
	
	player_play_stream(gtk_entry_get_text(GTK_ENTRY(urlinput)));
	
}
