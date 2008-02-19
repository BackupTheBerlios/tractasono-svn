/*
 *      utils.h
 *      
 *      Copyright 2007 Patrik Obrist <patrik@gmx.net>
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

#ifndef UTILS_H
#define UTILS_H


// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <tag_c.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-utils.h>

// Defines
#define BUFFER_SIZE		9216

// Prototypen
void create_dir (const gchar *path);
gchar *get_tractasono_dir (void);
gchar *get_music_dir (void);
gchar *get_artist_dir (gchar *artist);
void create_artist_dir (gchar *artist);
gchar *get_album_dir (gchar *album, gchar *artist);
void create_album_dir (gchar *album, gchar *artist);
gchar *get_track_path (gchar *track, gchar *album, gchar *artist);
gchar *get_track_name (gchar *title, gchar *artist, gint tracknr, gchar *extension);
gchar *get_file_extension (gchar *path);
gboolean copy_file (const gchar *source, const gchar *target);
gboolean exist_target (const gchar *source, const gchar *target);


#endif
