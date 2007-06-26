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

#include "settings.h"
#include "interface.h"
#include "keyboard.h"

GConfClient *client;

/* This is called when our cdrom key is changed. */
static void gconf_notify_cdrom(GConfClient *client, guint cnxn_id, GConfEntry  *entry, gpointer user_data)
{
	GConfValue *value = gconf_entry_get_value (entry);
	const char* path = gconf_value_get_string(value);
	g_print("Settings CDROM changed: %s\n", path);
}

/* This is called when our cdrom key is changed. */
static void gconf_notify_burner(GConfClient *client, guint cnxn_id, GConfEntry  *entry, gpointer user_data)
{
	GConfValue *value = gconf_entry_get_value (entry);
	const char* path = gconf_value_get_string(value);
	g_print("Settings Burner changed: %s\n", path);
}

void settings_init()
{
	client = gconf_client_get_default ();

	/* Add our directory to the list of directories the GConfClient will
	 * watch.
	 */
	gconf_client_add_dir (client, DRIVEPATH, GCONF_CLIENT_PRELOAD_NONE, NULL);

	/* Listen to changes to our key. */
	gconf_client_notify_add (client, CDROMKEY, gconf_notify_cdrom, NULL, NULL, NULL);
	gconf_client_notify_add (client, BURNERKEY, gconf_notify_burner, NULL, NULL, NULL);

	g_print("GConf initialized!\n");
}

void settings_set_cdrom(GnomeVFSDrive *drive)
{
	if (drive) {
		g_print("CDROM wird gesetzt!\n");
		cdrom = drive;
		char* path = gnome_vfs_drive_get_device_path (drive);
		gconf_client_set_string(client, CDROMKEY, path, NULL);
	}
}

GnomeVFSDrive* settings_get_cdrom()
{
	return cdrom;
}

void settings_set_burner(GnomeVFSDrive *drive)
{
	if (drive) {
		g_print("Burner wird gesetzt!\n");
		burner = drive;
		char* path = gnome_vfs_drive_get_device_path (drive);
		gconf_client_set_string(client, BURNERKEY, path, NULL);
	}
}

GnomeVFSDrive* settings_get_burner()
{
	return burner;
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


