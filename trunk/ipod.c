#include <gpod/ipod-device.h>
#include <gpod/itdb.h>
#include <stdio.h>

// Callback für Tracktree durchsuchen
gboolean tracktree_traverse (gpointer key, gpointer value, gpointer data)
{
	Itdb_Track *track = NULL;

	// Track holen
	track = (Itdb_Track*)value;
	if (track != NULL) {

		// Track Start
		printf("\n<< Track Id %i >>\n", track->id);

		// Ipodpfad aud Unixpfad ändern
		itdb_filename_ipod2fs (track->ipod_path);
		printf("   Pfad: %s\n", track->ipod_path);

		printf("   Artist: %s\n", track->artist);
		printf("   Titel: %s\n", track->title);
		printf("   Album: %s\n", track->album);
		printf("   Genre: %s\n", track->genre);
		printf("   Dateityp: %s\n", track->filetype);

		// Track Ende
		printf("\n");

	} else {
		printf("Fehler: Track konnte nicht initialisiert werden!\n");
	}

	return FALSE;
}

// Gebe alle verfügbaren Ipods zurück
// HAL wird benötigt
GList *ipod_get_devices()
{
	GList *devices = NULL;
	GList *device = NULL;
	IpodDevice *ipod = NULL;

	devices = itdb_device_list_devices();

	for (device = g_list_first(devices); device != NULL; device = g_list_next(devices)) {
		ipod = (IpodDevice*)device->data;
		if (ipod != NULL) {
			// Sollte noch implementiert werden
		}
	}

	return devices;
}

// Liste Infos aus der iTunesDB
int inspect_ipod(char *mountpoint)
{
	Itdb_iTunesDB *db = NULL;
	guint32 trackcount = 0;
	guint32 trackcount_nontransferred = 0;
	guint32 playlistcount = 0;
	GTree *tracktree = NULL;

	// Listing starten
	printf("\nIpod-Listener Start!\n");

	db = itdb_parse (mountpoint, NULL);
	if (db == NULL) {
		printf("iTunesDB konnte nicht initialisiert werden!\n");
		return 1;
	}

	// iTunesDB Pointer ausgeben
	printf("iTunesDB Pointer: %p\n", db);

	// Anzahl Tracks auslesen
	trackcount = itdb_tracks_number (db);
	printf("Anzahl Tracks: %i\n", trackcount);

	// Anzahl noch nicht transferierte Tracks auslesen
	trackcount_nontransferred = itdb_tracks_number_nontransferred (db);
	printf("Anzahl noch nicht transferierte Tracks: %i\n", trackcount_nontransferred);

	// Anzahl Playlists auslesen
	playlistcount = itdb_playlists_number (db);
	printf("Anzahl Playlists: %i\n", playlistcount);


	// Tracktree holen
	tracktree = itdb_track_id_tree_create (db);
	if (tracktree == NULL) {
		printf("Tracktree konnte nicht initialisiert werden!\n");
		return 1;
	}
	g_tree_foreach (tracktree, tracktree_traverse, NULL);


	printf("Ipod-Listener Ende!\n\n");

	return 0;
}
