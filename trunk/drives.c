#include "drives.h"

int init_drives()
{
	if (!gnome_vfs_init ()) {
		printf ("Could not initialize GnomeVFS\n");
		return DRIVES_FAIL;
	}

	GnomeVFSVolumeMonitor* monitor = NULL;
	monitor = gnome_vfs_get_volume_monitor();

	g_signal_connect(monitor, "drive-connected", G_CALLBACK(drive_connected), NULL);
	//g_signal_connect(monitor, "drive-disconnected", G_CALLBACK(drive_disconnected), NULL);

	printf ("Initialize GnomeVFS\n");
	return DRIVES_SUCCESS;
}

// Drive-Connected Signal
void drive_connected(GnomeVFSVolumeMonitor *monitor, GnomeVFSDrive *drive, gpointer data)
{
	char *drivename;

	drivename = gnome_vfs_drive_get_display_name(drive);

	if (gnome_vfs_drive_is_connected(drive)) {
		g_print("Drive \"%s\" connected!\n", drivename);
	} else {
		g_print("Drive \"%s\" disconnected!\n", drivename);
	}
}

// Drive-Disconnected Signal
void drive_disconnected(GnomeVFSVolumeMonitor *monitor, GnomeVFSDrive *drive, gpointer data)
{
	char *drivename;

	drivename = gnome_vfs_drive_get_display_name(drive);
	
	if (!gnome_vfs_drive_is_connected(drive)) {
		g_print("Drive \"%s\" disconnected!\n", drivename);
	}
}

int print_error (GnomeVFSResult result, const char *uri_string)
{
	const char *error_string;

	/* get the string corresponding to this GnomeVFSResult value */
	error_string = gnome_vfs_result_to_string (result);
	printf ("Error %s occured opening location %s\n", error_string, uri_string);
	
	return DRIVES_FAIL;
}
