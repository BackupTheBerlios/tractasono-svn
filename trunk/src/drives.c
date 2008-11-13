/*
 *      drives.c
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

#include "drives.h"
#include "disc.h"
#include "musicbrainz.h"

#include <gio/gio.h>

GVolumeMonitor *monitor;


// Drive-Connected Signal
void drive_connected (GVolumeMonitor *monitor, GDrive *drive, gpointer data)
{
	gchar *name;
	
	name = g_drive_get_name (drive);
	
	g_debug ("%s connected!", name);
}

// Drive-Disconnected Signal
void drive_disconnected (GVolumeMonitor *monitor, GDrive *drive, gpointer data)
{
	gchar *name;
	
	name = g_drive_get_name (drive);
	
	g_debug ("%s disconnected!", name);
}


int drives_init()
{
	g_message ("Drives init");
	
	monitor = g_volume_monitor_get ();
	
	g_signal_connect (monitor, "drive-connected", G_CALLBACK(drive_connected), NULL);
	g_signal_connect (monitor, "drive-disconnected", G_CALLBACK(drive_disconnected), NULL);
	
	return TRUE;
}
