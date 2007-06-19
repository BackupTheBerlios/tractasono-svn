#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

// Globale Includes
#include <getopt.h>

// Lokale Includes
#include "tractasono.h"
#include "settings.h"
//#include "ipod.h"
//#include "database.h"
#include "drives.h"
#include "player.h"
#include "interface.h"





// Programmeinstieg
int main(int argc, char *argv[])
{
	#ifdef ENABLE_NLS
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		textdomain (PACKAGE);
	#endif
	
	// Variablen initialisieren
	xml = NULL;
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
