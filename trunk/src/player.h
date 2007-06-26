/*
 *      player.h
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
