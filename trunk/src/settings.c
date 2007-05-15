#include "settings.h"

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
