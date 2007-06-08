#include "drives.h"
#include "settings.h"


// Drive-Connected Signal
void drive_connected(GnomeVFSVolumeMonitor *monitor, GnomeVFSDrive *drive, gpointer data)
{
	if (gnome_vfs_drive_is_user_visible (drive)) {
		char *drivename;
		drivename = gnome_vfs_drive_get_display_name(drive);
		g_print("Drive \"%s\" connected!\n", drivename);
		//print_drive(drive);
		settings_set_cdrom(drive);
	}
}

// Drive-Disconnected Signal
void drive_disconnected(GnomeVFSVolumeMonitor *monitor, GnomeVFSDrive *drive, gpointer data)
{
	if (gnome_vfs_drive_is_user_visible (drive)) {
		char *drivename;
		drivename = gnome_vfs_drive_get_display_name(drive);
		g_print("Drive \"%s\" disconnected!\n", drivename);
		//print_drive(drive);
	}
}

int drives_init()
{
	if (!gnome_vfs_init ()) {
		printf ("Could not initialize GnomeVFS\n");
		return DRIVES_FAIL;
	}

	GnomeVFSVolumeMonitor* monitor = NULL;
	monitor = gnome_vfs_get_volume_monitor();

	g_signal_connect(monitor, "drive-connected", G_CALLBACK(drive_connected), NULL);
	g_signal_connect(monitor, "drive-disconnected", G_CALLBACK(drive_disconnected), NULL);

	printf ("Initialize GnomeVFS\n");
	return DRIVES_SUCCESS;
}

/*static void print_drive(GnomeVFSDrive *drive)
{
	char *path, *uri, *name, *icon;
	GnomeVFSVolume *volume;

	path = gnome_vfs_drive_get_device_path (drive);
	uri = gnome_vfs_drive_get_activation_uri (drive);
	icon = gnome_vfs_drive_get_icon (drive);
	name = gnome_vfs_drive_get_display_name (drive);
	volume = gnome_vfs_drive_get_mounted_volume (drive);
	
	g_print ("drive(%p)[dev: %s, mount: %s, device type: %d, icon: %s, name: %s, user_visible: %d, volume: %p]\n",
		 drive,
		 path?path:"(nil)",
		 uri,
		 gnome_vfs_drive_get_device_type (drive),
		 icon,
		 name,
		 gnome_vfs_drive_is_user_visible (drive),
		 volume);
	g_free (path);
	g_free (uri);
	g_free (icon);
	g_free (name);
	gnome_vfs_volume_unref (volume);
}

static void print_volume(GnomeVFSVolume *volume)
{
	char *path, *uri, *name, *icon;
	GnomeVFSDrive *drive;

	path = gnome_vfs_volume_get_device_path (volume);
	uri = gnome_vfs_volume_get_activation_uri (volume);
	icon = gnome_vfs_volume_get_icon (volume);
	name = gnome_vfs_volume_get_display_name (volume);
	drive = gnome_vfs_volume_get_drive (volume);
	g_print ("vol(%p)[dev: %s, mount: %s, device type: %d, handles_trash: %d, icon: %s, name: %s, user_visible: %d, drive: %p]\n",
		 volume,
		 path?path:"(nil)",
		 uri,
		 gnome_vfs_volume_get_device_type (volume),
		 gnome_vfs_volume_handles_trash (volume),
		 icon,
		 name,
		 gnome_vfs_volume_is_user_visible (volume),
		 drive);
	g_free (path);
	g_free (uri);
	g_free (icon);
	g_free (name);
	gnome_vfs_drive_unref (drive);
}*/

int print_error (GnomeVFSResult result, const char *uri_string)
{
	const char *error_string;

	/* get the string corresponding to this GnomeVFSResult value */
	error_string = gnome_vfs_result_to_string (result);
	printf ("Error %s occured opening location %s\n", error_string, uri_string);
	
	return DRIVES_FAIL;
}