#include <libgnomevfs/gnome-vfs.h>
#include <gconf/gconf-client.h>

GnomeVFSDrive *cdrom;
GnomeVFSDrive *burner;

#define DRIVEPATH "/apps/tractasono/drives"
#define CDROMKEY "/apps/tractasono/drives/cdrom"
#define BURNERKEY "/apps/tractasono/drives/burner"

void settings_init();
void settings_set_cdrom(GnomeVFSDrive *drive);
void settings_set_burner(GnomeVFSDrive *drive);
GnomeVFSDrive* settings_get_cdrom();
GnomeVFSDrive* settings_get_burner();
