/*
 *      settings.h
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

#include <libgnomevfs/gnome-vfs.h>
#include <gconf/gconf-client.h>

GnomeVFSDrive *cdrom;
GnomeVFSDrive *burner;

#define DRIVEPATH "/apps/tractasono/drives"
#define CDROMKEY "/apps/tractasono/drives/cdrom"
#define BURNERKEY "/apps/tractasono/drives/burner"

void settings_init();
void settings_set_cdrom(GnomeVFSDrive *drive);
void settings_set_burner(GnomeVFSDrive *drive);
GnomeVFSDrive* settings_get_cdrom();
GnomeVFSDrive* settings_get_burner();
