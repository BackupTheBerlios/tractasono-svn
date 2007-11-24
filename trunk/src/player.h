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

#ifndef PLAYER_H
#define PLAYER_H


typedef enum {
  STATE_PLAY_LOCAL,
  STATE_PLAY_STREAM,
  STATE_PLAY_NOTHING
} PlayerState;

// Includes
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/tag/tag.h>

// Prototypen
gint player_test (void);

void player_init (int argc, char *argv[]);

void player_set_play (void);
void player_set_pause (void);
void player_set_stop (void);

void player_play_uri (const gchar *uri);

gboolean player_seek_to_position (gint64 position);

gboolean player_get_playing (void);
PlayerState player_get_state (void);

gint64 player_get_duration (void);
gint64 player_get_position (void);

gint64 ns_to_seconds (gint64 ns);
gchar* ns_formatted (gint64 ns);

#endif
