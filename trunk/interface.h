// Globale Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

// Public Functions
void interface_init(int *argc, char ***argv);

void interface_load(const gchar *gladefile);

void interface_set_songinfo(const gchar *artist,
							const gchar *title,
							gdouble seconds);

void interface_set_position_in_song(gdouble position);
