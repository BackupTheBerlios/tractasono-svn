/*
 *      upnp-cp.c
 *      
 *      Copyright 2008 Patrik Obrist <patrik@gmx.net>
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

#include "upnp-cp.h"
#include <glib.h>



// Tritt auf, wenn das Gerät erscheint
static void device_proxy_available_cb (GUPnPControlPoint *control_point,
									   GUPnPDeviceProxy  *proxy,
									   gpointer user_data)
{
	GUPnPDeviceInfo *info;
	gchar *name;
	
	info = (GUPnPDeviceInfo*) proxy;
	
	name = gupnp_device_info_get_friendly_name (info);
	g_debug ("MediaServer '%s' gefunden!", name);
	
	g_free (name);
}


// Tritt auf, wenn das Gerät verschwindet
static void device_proxy_unavailable_cb (GUPnPControlPoint *control_point,
									   GUPnPDeviceProxy  *proxy,
									   gpointer user_data)
{
	GUPnPDeviceInfo *info;
	gchar *name;
	
	info = (GUPnPDeviceInfo*) proxy;
	
	name = gupnp_device_info_get_friendly_name (info);
	g_debug ("MediaServer '%s' verloren!", name);
	
	g_free (name);
}


// UPNP Initialisieren
void upnp_init (int argc, char *argv[])
{
	g_message ("UPNP init");
	
	GUPnPContext *context;
	GUPnPControlPoint *cp;
	
	// Required initialisation
	g_thread_init (NULL);
	
	// Create a new GUPnP Context
    context = gupnp_context_new (NULL, NULL, 0, NULL);

	// Create a Control Point
	cp = gupnp_control_point_new (context, "urn:schemas-upnp-org:device:MediaServer:1");
	
	// Connect Signals
    g_signal_connect (cp, "device-proxy-available", G_CALLBACK (device_proxy_available_cb), NULL);
    g_signal_connect (cp, "device-proxy-unavailable", G_CALLBACK (device_proxy_unavailable_cb), NULL);

	// Tell the Control Point to start searching
	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp), TRUE);
}
