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



// Globale Variablen
GladeXML *glade;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;
GtkWidget *vbox_tractasono;

GtkEntry *actual_entry;
GtkProgressBar *progress;


typedef struct {
	GtkWidget *music;
	GtkWidget *radio;
	GtkWidget *import;
	GtkWidget *settings;
	GtkWidget *fullscreen;
	GtkWidget *disc;
} WindowModule;

WindowModule module;






// Public Functions
void interface_init(int *argc, char ***argv);

void interface_load(const gchar *gladefile);

void interface_clean();

void interface_clean_all();

void interface_show_module(GtkWidget *widget);

void interface_set_songinfo(const gchar *artist, const gchar *title, const gchar *uri);

void interface_set_song_position(gint64 position);

void interface_set_song_duration(gint64 duration);

void interface_set_playing(gboolean isplaying);

void interface_set_playimage(const gchar *stock_id);
