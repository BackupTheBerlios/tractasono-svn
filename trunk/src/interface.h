/*
 *      interface.h
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

// Globale Includes
#include <gtk/gtk.h>
#include <glade/glade.h>


// Strukturen

typedef struct {
	GtkWidget *music;
	GtkWidget *radio;
	GtkWidget *ipod;
	GtkWidget *import;
	GtkWidget *settings;
	GtkWidget *fullscreen;
	GtkWidget *disc;
	GtkWidget *previous;
} WindowModule;


// Globale Variablen
GladeXML *glade;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *keyboard;
GtkWidget *vbox_tractasono;
GtkWidget *lcd;

GtkEntry *actual_entry;
GtkProgressBar *progress;

WindowModule module;


// Public Functions
void interface_init (int argc, char *argv[]);
void interface_clean ();
void interface_clean_all ();
void interface_show_module (GtkWidget *widget);
void interface_show_previous_module ();
void interface_set_songinfo (const gchar *artist, const gchar *title, const gchar *uri);
void interface_set_song_position (gint64 position);
void interface_set_song_duration (gint64 duration);
void interface_set_playing (gboolean isplaying);

GtkStyle* interface_create_style (GdkColor *fg, GdkColor *bg, gboolean do_grade);
GdkColor* interface_create_color (int red, int green, int blue);

