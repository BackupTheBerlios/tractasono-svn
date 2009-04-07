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
void volume_added (GVolumeMonitor *volume_monitor, GVolume *volume, gpointer data)
{
	gchar *name;
	name = g_volume_get_name (volume);
	g_debug ("%s connected!", name);
	g_free (name);
}

// Drive-Disconnected Signal
void volume_removed (GVolumeMonitor *volume_monitor, GVolume *volume, gpointer data)
{
	gchar *name;
	name = g_volume_get_name (volume);
	g_debug ("%s disconnected!", name);
	g_free (name);
}


int drives_init()
{
	g_message ("Drives init");
	
	monitor = g_volume_monitor_get ();
	
	g_signal_connect (monitor, "volume-added", G_CALLBACK(volume_added), NULL);
	g_signal_connect (monitor, "volume-removed", G_CALLBACK(volume_removed), NULL);
	
	return TRUE;
}
