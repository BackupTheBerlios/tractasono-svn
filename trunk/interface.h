// Globale Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

// Public Functions
void interface_init(int *argc, char ***argv);

void interface_load(const gchar *gladefile);

void interface_set_songinfo(const gchar *artist,
							const gchar *title,
							gdouble seconds);

void interface_set_song_position(gdouble position);
void interface_set_song_duration(gdouble duration);

void interface_set_slidermove(gboolean move);
gboolean interface_get_slidermove();

void interface_set_playing(gboolean isplaying);
gboolean interface_get_playing();

