// Globale Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

// Public Functions
void interface_init(int *argc, char ***argv);

void interface_load(const gchar *gladefile);

void interface_clean();

void interface_clean_all();

void interface_show_module(GtkWidget *widget);

void interface_set_songinfo(const gchar *artist,
							const gchar *title,
							gdouble seconds);

void interface_set_song_position(gint64 position);
void interface_set_song_duration(gint64 duration);

void interface_set_slidermove(gboolean move);
gboolean interface_get_slidermove();

void interface_set_playing(gboolean isplaying);
gboolean interface_get_playing();

void interface_set_playimage(const gchar *stock_id);


gdouble gint64_to_double(gint64 value);
