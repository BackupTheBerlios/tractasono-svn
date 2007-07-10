/*
 *      settings.c
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

#include <string.h>

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "settings.h"
#include "interface.h"
#include "keyboard.h"

 

void settings_init()
{
	g_message ("Settings init");
	
	settings = g_slice_new (Settings);
	settings_read ();
}





void settings_read (void)
{
	GKeyFileFlags flags;
	GError *error = NULL;
	
	settings->keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	g_key_file_load_from_file (settings->keyfile, settings_get_configfile (), flags, &error);
	if (error) {
		g_warning ("config file konnte nicht gelesen werden! -> %s", error->message);
	}
	
	settings->general_log = g_key_file_get_boolean (settings->keyfile, "general", "log", NULL);
	settings->database_name = g_key_file_get_string (settings->keyfile, "database", "name", NULL);
}


gchar* settings_get_configfile ()
{
	GString *configfile;
	
	configfile = g_string_new (NULL);
	g_string_printf (configfile, "%s/.%s", g_get_home_dir (), PACKAGE);
	
	return configfile->str;
}




// Handler für Fukuswechler auf dem Settings-Reiter
void on_notebook1_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	gchararray name;
	g_object_get(notebook, "name", &name, NULL);
	g_print("Page name: %s\n", name);

	g_print("Reiter gewechselt, schliesse Tastatur\n");
	keyboard_show(FALSE);
}

