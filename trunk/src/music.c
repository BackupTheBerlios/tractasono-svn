#include "music.h"
#include "player.h"


void on_button_read_toc_clicked(GtkButton *button, gpointer user_data)
{
	g_print("Read TOC wurde gedrückt\n");
	player_play_testfile();
}
