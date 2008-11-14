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



static void service_proxy_available_cb (GUPnPControlPoint *cp,
                            GUPnPServiceProxy *proxy,
                            gpointer           userdata)
{
  GError *error = NULL;
  char *ip = NULL;
  
  gupnp_service_proxy_send_action (proxy,
				   /* Action name and error location */
				   "GetExternalIPAddress", &error,
				   /* IN args */
				   NULL,
				   /* OUT args */
				   "NewExternalIPAddress",
				   G_TYPE_STRING, &ip,
				   NULL);
  
  if (error == NULL) {
    g_print ("External IP address is %s\n", ip);
    g_free (ip);
  } else {
    g_printerr ("Error: %s\n", error->message);
    g_error_free (error);
  }
  //g_main_loop_quit (main_loop);
}


void upnp_init (int argc, char *argv[])
{
	/* Required initialisation */
	g_thread_init (NULL);

	GUPnPContext *context;
	GUPnPControlPoint *cp;
	
	/* Create a new GUPnP Context.  By here we are using the default GLib main
     context, and connecting to the current machine's default IP on an
     automatically generated port. */
    context = gupnp_context_new (NULL, NULL, 0, NULL);

	/* Create a Control Point targeting WAN IP Connection services */
	cp = gupnp_control_point_new (context, "urn:schemas-upnp-org:service:WANIPConnection:1");
	
	/* The service-proxy-available signal is emitted when any services which match
     our target are found, so connect to it */
    g_signal_connect (cp, "service-proxy-available", G_CALLBACK (service_proxy_available_cb), NULL);

	/* Tell the Control Point to start searching */
	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp), TRUE);


	
}
