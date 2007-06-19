#include "fullscreen.h"

void fullscreen_init()
{
	g_print("Fullscreen Modul wird initialisiert...\n");
	
}

void on_fullscreen_button_close_clicked(GtkWidget *widget, gpointer user_data)
{
	g_print("Test\n");
	
}

void on_window_fullscreen_button_press_event(GtkWidget *widget, gpointer user_data)
{
	// Hier sollte noch etwas Code rein
	g_print("Fullscreen back gedrückt!\n");
	
}
