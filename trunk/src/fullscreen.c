/*
 *      fullscreen.c
 *      
 *      Copyright 2007 Patrik Obrist <padx@gmx.net>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "fullscreen.h"
#include "interface.h"


void fullscreen_init (void)
{
	g_message ("\tFullscreen Modul init");
	
}

void fullscreen_load (void)
{
	
	
}

void on_fullscreen_button_close_clicked (GtkWidget *widget, gpointer user_data)
{
	g_message ("Vollbild Modus verlassen");
	
	interface_show_previous_module ();
}


void on_vbox_fullscreen_button_press_event (GtkWidget      *widget,
                                            GdkEventButton *event,
                                            gpointer        user_data)
{
	g_message ("Test 2");
	
}                                            

void on_eventbox_fullscreen_button_press_event (GtkWidget      *widget,
                                            GdkEventButton *event,
                                            gpointer        user_data)
{
	g_message ("Vollbild Modus verlassen");
	
	interface_show_previous_module ();
	
}   



void on_eventbox1_button_press_event (GtkWidget      *widget,
                                            GdkEventButton *event,
                                            gpointer        user_data)
{
	g_message ("Kleine Eventbox");
	
}  
