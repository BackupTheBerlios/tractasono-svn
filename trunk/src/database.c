/*
 *      database.c
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <libgda/libgda.h>

#include "database.h"


void database_init (int argc, char *argv[])
{
	gda_init (PACKAGE, VERSION, argc, argv);
	
	database_list_providers();
	database_list_sources();
	database_testfunc();
}


void database_list_providers (void)
{
    GList *prov_list;
    GList *node;
    GdaProviderInfo *info;

    prov_list = gda_config_get_provider_list ();

    for (node = g_list_first (prov_list); node != NULL; node = node->next) {
        info = (GdaProviderInfo *) node->data;
        g_print ("ID: %s\n", info->id);
    }

    gda_config_free_provider_list (prov_list);
}


void database_list_sources (void)
{
    GList *ds_list;
    GList *node;
    GdaDataSourceInfo *info;

    ds_list = gda_config_get_data_source_list ();
    g_print ("\n");
    for (node = g_list_first (ds_list); node != NULL; node = g_list_next (node)) {
        info = (GdaDataSourceInfo *) node->data;

        g_print ("NAME: %s PROVIDER: %s CNC: %s DESC: %s USER: %s PASSWORD: %s\n",
                 info->name, info->provider, info->cnc_string, info->description,
                 info->username, info->password);
    }
    g_print ("\n");

    gda_config_free_data_source_list (ds_list);
}


void database_testfunc (void)
{
	GError *error;
	GdaClient *client;
	GdaConnection *connection;
      
	client = gda_client_new ();
      
	g_print ("CONNECTING\n");
	connection = gda_client_open_connection (client, "tractasono", NULL, NULL,
						 GDA_CONNECTION_OPTIONS_READ_ONLY, &error);

	g_print ("CONNECTED\n");
      
	//execute_some_queries (connection);

	g_print ("ERRORS PROVED!\n");
      
	//process_accounts (connection);
      
	g_object_unref (G_OBJECT (client));
}





