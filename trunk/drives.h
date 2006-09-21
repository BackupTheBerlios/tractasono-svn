#include <stdio.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-utils.h>

#define DRIVES_SUCCESS 0
#define DRIVES_FAIL 1

int init_drives();
void drive_connected(GnomeVFSVolumeMonitor *monitor, GnomeVFSDrive *drive, gpointer data);
void drive_disconnected(GnomeVFSVolumeMonitor *monitor, GnomeVFSDrive *drive, gpointer data);
int print_error (GnomeVFSResult result, const char *uri_string);
