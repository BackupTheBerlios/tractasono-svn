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


void radio_init(void)
{
	g_message ("\tRadio Modul init");
	
}

void on_button_radio_stream_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Radio Stream...\n");
	
	GtkWidget *urlinput;
	urlinput = glade_xml_get_widget(glade, "entry_radio_url");
	
	player_play_uri(gtk_entry_get_text(GTK_ENTRY(urlinput)));
}


static void xml_start(GMarkupParseContext *context,
					  const gchar *element_name,
					  const gchar **attribute_names,
					  const gchar **attribute_values,
					  gpointer data,
					  GError **error)
{
	const gchar *element;

	element = g_markup_parse_context_get_element(context);
	if (g_ascii_strcasecmp(element, "genre") == 0) {
		g_debug("genre: %s", attribute_values[0]);
	}
}


static void xml_end(GMarkupParseContext *context,
					const gchar *element_name,
					gpointer data,
					GError **error)
{
	const gchar *element;

	element = g_markup_parse_context_get_element(context);
}


static void xml_element(GMarkupParseContext *context,
						const gchar *text,
						gsize text_len,
						gpointer data,
						GError **error)
{
  const gchar *element;

  element = g_markup_parse_context_get_element(context);  
}


/* Handle errors that occur in parsing the XML file. */
static void xml_err(GMarkupParseContext *context,
					GError *error,
					gpointer data)
{
	g_critical("%s", error->message);
}




void on_button_radio_fetch_station_clicked(GtkWidget *widget, gpointer user_data)
{
	g_debug("Fetch radio stations");
	
	
	gchar *content;
  	
  	// http://www.shoutcast.com/sbin/newxml.phtml
  	// /home/patrik/Projekte/tractasono/src/data/genres.xml
  	g_file_get_contents("/home/patrik/Projekte/tractasono/src/data/genres.xml", &content, NULL, NULL);
	
	static GMarkupParser parser = { xml_start, xml_end, xml_element, NULL, xml_err };
	GMarkupParseContext *context;
	
	context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
	g_markup_parse_context_parse (context, content, -1, NULL);
	g_markup_parse_context_free (context);

}

