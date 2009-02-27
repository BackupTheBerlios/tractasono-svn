/*
 *      main.c
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

// Lokale Includes
#include "settings.h"
#include "drives.h"
#include "interface.h"
#include "database.h"
#include "musicbrainz.h"
#include "player.h"

#include <libgnomevfs/gnome-vfs.h>

// Programmeinstieg
int main (int argc, char *argv[])
{
	// Startmeldung
	g_message ("<== "PACKAGE" "VERSION" starting ==>");
	
	// Externe Module initialisieren
	gtk_init (&argc, &argv);
	gnome_vfs_init ();
	
	// Interne Module initialisieren
	db_init (argc, argv);
	musicbrainz_init ();
	drives_init ();
	player_init (argc, argv);
	interface_init (argc, argv);

	// Programmloop starten
	gtk_main ();
	
	// Datenbank schliessen
	db_close ();

	// Shutdown Meldung
	g_message ("<== "PACKAGE" shutting down ==>");

	return 0;
}
