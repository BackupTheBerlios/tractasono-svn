/*
 *      server.c
 *      
 *      Copyright 2009 Patrik Obrist <patrik@valhalla>
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


#include <gtk/gtk.h>
#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-av/gupnp-av.h>
#include "server.h"
#include "interface.h"


GtkTreeStore *server_store;

enum {
	NAME,
	COLUMNS
};

typedef struct {
	GUPnPServiceProxy *service;
	gchar *id;
	GtkTreeIter iter;
} ParentInfo;

void setup_tree_view (GtkWidget *treeview);
ParentInfo* tree_add_device (const char *name);
ParentInfo* tree_add_node (const char *name, ParentInfo *info);
void browse (ParentInfo *info);
void setup_control_point ();
void add_device (const char *name);


ParentInfo* parentinfo_new ()
{
	ParentInfo *info;
	info = g_new0 (ParentInfo, 1);
	info->service = NULL;
	info->id = NULL;
	return info;
}

void setup_tree_view (GtkWidget *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview),
									   FALSE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW (treeview),
										 TRUE);
	
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
				("Name", renderer, "text", NAME, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
	
	server_store = gtk_tree_store_new (COLUMNS, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview),
							 GTK_TREE_MODEL (server_store));
	g_object_unref (server_store);
}


void on_node_parsed (GUPnPDIDLLiteParser *parser, xmlNode *node,
					 gpointer user_data)
{
	ParentInfo *info = user_data;
	char *id = gupnp_didl_lite_object_get_id (node);
	char *title = gupnp_didl_lite_object_get_title (node);
	guint childs = gupnp_didl_lite_container_get_child_count (node);
	if (title) {
		info = tree_add_node (title, info);
		info->id = g_strdup (id);
	}
	if (childs > 0) {
		browse (info);
	}
}


void browse (ParentInfo *info)
{
	GError *error = NULL;
	char *didl;
	int number_returned;
	int total_matches;
	gupnp_service_proxy_send_action (info->service,
		/* Action name and error location */
		"Browse", &error,
		/* IN args */
		"ObjectID",       G_TYPE_STRING, info->id,
		"BrowseFlag",     G_TYPE_STRING, "BrowseDirectChildren",
		"Filter",         G_TYPE_STRING, "*",
		"StartingIndex",  G_TYPE_UINT,   0,
		"RequestedCount", G_TYPE_UINT,   0,
		"SortCriteria",   G_TYPE_STRING, "",
		NULL,
		/* OUT args */
		"Result",         G_TYPE_STRING, &didl,
		"NumberReturned", G_TYPE_UINT,   &number_returned,
		"TotalMatches",   G_TYPE_UINT,   &total_matches,
		NULL);

	if (error == NULL) {
		gboolean bOk;
		GUPnPDIDLLiteParser* parser = gupnp_didl_lite_parser_new ();
		bOk = gupnp_didl_lite_parser_parse_didl (parser, didl,
												 on_node_parsed,
												 info, &error);
		if (!bOk) {
			g_warning ("DIDL Error: %s", error->message);
			g_error_free (error);
		}
	} else {
		g_printerr ("Error: %s\n", error->message);
		g_error_free (error);
	}
}


void on_device_available (GUPnPControlPoint *cp,
						  GUPnPDeviceProxy *device, 
						  gpointer user_data)
{
	const char *name;
	GUPnPServiceInfo *service_info;
	
	name = gupnp_device_info_get_friendly_name (GUPNP_DEVICE_INFO (device));
	
	service_info = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (device),
				   "urn:schemas-upnp-org:service:ContentDirectory:1");
	
	ParentInfo *info = tree_add_device (name);
	info->service = GUPNP_SERVICE_PROXY (service_info);
	browse (info);
	
	//add_device (name);
}


void on_combobox_server_changed (GtkComboBox *widget,
								 gpointer user_data)
{
	g_message ("Combo changed!");
}


void add_device (const char *name)
{
	//on_combobox_server_changed
	
	GtkWidget *combo;
	combo = interface_get_widget ("combobox_server");
	
	GtkTreeModel *model;
	model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
	
	//GtkTreeIter iter;
	//gtk_list_store_append (GTK_LIST_STORE (model), &iter);
	//gtk_list_store_set (GTK_LIST_STORE (model), &iter, NAME, name, -1);
	
	/*ParentInfo *info = parentinfo_new ();
	info->id = g_strdup ("0");
	gtk_tree_store_append (server_store, &info->iter, NULL);
	gtk_tree_store_set (server_store, &info->iter, NAME, name, -1);
	return info;*/
}


ParentInfo* tree_add_device (const char *name)
{
	ParentInfo *info = parentinfo_new ();
	info->id = g_strdup ("0");
	gtk_tree_store_append (server_store, &info->iter, NULL);
	gtk_tree_store_set (server_store, &info->iter, NAME, name, -1);
	return info;
}


ParentInfo* tree_add_node (const char *name, ParentInfo *info)
{
	ParentInfo *info_new = parentinfo_new ();
	info_new->service = info->service;
	gtk_tree_store_append (server_store, &info_new->iter, &info->iter);
	gtk_tree_store_set (server_store, &info_new->iter, NAME, name, -1);
	return info_new;
}


void setup_control_point ()
{
	GUPnPContext *context;
	GUPnPControlPoint *cp;
	
	if (!g_thread_get_initialized ()) {
		g_thread_init (NULL);
	}
	context = gupnp_context_new (NULL, NULL, 0, NULL);

	cp = gupnp_control_point_new (context,
						"urn:schemas-upnp-org:device:MediaServer:1");
	g_signal_connect (cp, "device-proxy-available",
					  G_CALLBACK (on_device_available), NULL);
	
	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp),
									   TRUE);
}


void server_init (void)
{
	g_message ("\tServer Modul init");
	
	GtkWidget *treeview;
	treeview = interface_get_widget ("treeview_server");
	setup_tree_view (treeview);
	
	setup_control_point ();
}
