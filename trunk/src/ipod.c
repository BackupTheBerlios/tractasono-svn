/*
 *      ipod.c
 *      
 *      Copyright 2007 Patrik Obrist <patrik@valhalla>
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


#include "ipod.h"

#include <gpod/itdb.h>


// Prototypen
void ipod_testfunc (void);


void ipod_init (void)
{
	g_message ("iPod init");
	
}

void ipod_load (void)
{
	g_message ("iPod load");
	
	ipod_testfunc ();
}


void ipod_testfunc (void)
{
	Itdb_Device *device;
	Itdb_IpodInfo *info;
	gchar *mod, *gen;
	
	device = itdb_device_new ();
	
	itdb_device_set_mountpoint (device, "/media/SILVERBACK");
	if (!itdb_device_read_sysinfo (device)) {
		g_debug ("iPod sysinfo konnte nicht gelesen werden");
		return;
	}
	
	info = (Itdb_IpodInfo*) itdb_device_get_ipod_info (device);
	
	mod = (gchar*) itdb_info_get_ipod_model_name_string (info->ipod_model);
	gen = (gchar*) itdb_info_get_ipod_generation_string (info->ipod_generation);
	
	g_debug ("Ipod name: %s, generation: %s", mod, gen);
}




