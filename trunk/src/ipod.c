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
#include <ipoddevice/ipod-device.h>
#include <ipoddevice/ipod-device-event-listener.h>


// Prototypen
void ipod_testfunc (void);
void ipod_added (IpodDeviceEventListener *listener, const gchar *udi);
void ipod_removed (IpodDeviceEventListener *listener, const gchar *udi);
void ipod_list_devices (void);
void ipod_list_device_udis (void);
gint ipod_eject_device (gchar *id);


void ipod_init (void)
{
	IpodDeviceEventListener *listener;
	
	g_message ("iPod init");
	
	listener = ipod_device_event_listener_new ();
	
	g_signal_connect (listener, "device-added", G_CALLBACK(ipod_added), NULL);
	g_signal_connect (listener, "device-removed", G_CALLBACK(ipod_removed), NULL);
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


void ipod_added (IpodDeviceEventListener *listener, const gchar *udi)
{	
	IpodDevice *device;
    
    g_message ("Device Added: %s", udi);
    device = ipod_device_new (udi);
    
    if(device == NULL) {
        g_warning ("Not a Valid iPod! Try Rebooting the iPod");
    } else {
        ipod_device_debug (device);
        g_object_unref (device);
    }
}


void ipod_removed (IpodDeviceEventListener *listener, const gchar *udi)
{
	g_message ("Device Removed: %s", udi);
}


void ipod_list_devices (void)
{
    IpodDevice *device;
    GList *devices;
    gint i, n;

    devices = ipod_device_list_devices ();
    n = g_list_length (devices);
    
    if (n == 0) {
        g_warning ("No iPod devices present");
        return;
    }

    for (i = 0; i < n; i++) {
        device = (IpodDevice *) g_list_nth_data (devices, i);
        ipod_device_debug (device);
        g_object_unref (device);
    }

    g_list_free (devices);
}


void ipod_list_device_udis (void)
{
    gchar *udi;
    GList *devices;
    gint i, n;

    devices = ipod_device_list_device_udis ();
    n = g_list_length (devices);
    
    if (n == 0) {
        g_warning ("No iPod devices present");
        return;
    }

    for (i = 0; i < n; i++) {
        udi = (gchar *) g_list_nth_data (devices, i);
        g_message ("iPod UDI: %s\n", udi);
        g_free (udi);
    }

    g_list_free (devices);
}


gint ipod_eject_device (gchar *id)
{
    IpodDevice *device = ipod_device_new (id);
    gint ret = 1;
    
    if (device == NULL) {
        g_warning ("Could not get device for '%s'", id);
        return 1;
    }
    
    switch (ipod_device_eject (device, NULL)) {
        case EJECT_OK:
            g_message ("Ejected `%s' OK", id);
            ret = 0;
            break;
        case EJECT_ERROR:
            g_message ("Could not Eject `%s'", id);
            ret = 1;
            break;
        case EJECT_BUSY:
            g_message ("Could not Eject `%s', Device Busy", id);
            ret = 1;
            break;
    }
    
    g_object_unref (device);
    
    return ret;
}
