#include <gtk/gtk.h>
#include <gst/gst.h>

void player_init(int *argc, char **argv[]);

void player_set_play();
void player_set_stop();

void player_play_testfile();
void player_play_stream(const gchar *url);

void player_seek_to_position(gint seconds);

gboolean player_start_seek(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean player_stop_seek (GtkWidget *widget, gpointer user_data);

gboolean player_get_playing(void);

gint64 ns_to_seconds(gint64 ns);
gchar* ns_formatted(gint64 ns);
