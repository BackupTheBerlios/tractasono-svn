/*
 *      radio.c
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

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "radio.h"
#include "player.h"
#include "interface.h"

void on_button_radio_stream_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Radio Stream...\n");
	
	GtkWidget *urlinput;
	
	urlinput = glade_xml_get_widget(glade, "entry_radio_url");
	
	player_play_stream(gtk_entry_get_text(GTK_ENTRY(urlinput)));
	
}
