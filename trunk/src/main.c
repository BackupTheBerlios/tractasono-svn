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

// Globale Includes
#include <getopt.h>

// Lokale Includes
#include "settings.h"
#include "drives.h"
#include "player.h"
#include "interface.h"
#include "disc.h"

// He Mann, do hets vell zwenig Code!!

// Programmeinstieg
int main(int argc, char *argv[])
{
	#ifdef ENABLE_NLS
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		textdomain (PACKAGE);
	#endif
	
	// Variablen initialisieren
	glade = NULL;
	mainwindow = NULL;
	vbox_placeholder = NULL;
	vbox_keyboard = NULL;
	vbox_tractasono = NULL;

	actual_entry = NULL;

	// Interface initialisieren
	interface_init(&argc, &argv);

	// Player initialisieren
	player_init(&argc, &argv);
	
	// Settings initialisieren
	settings_init();

	// Drives initialisieren
	drives_init();

	// Das Interface laden
	interface_load("tractasono.glade");

	// Programmloop starten
	gtk_main();

	return 0;
}
