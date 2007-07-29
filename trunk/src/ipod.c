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
#include "interface.h"

#include <gpod/itdb.h>
#include <ipoddevice/ipod-device.h>
#include <ipoddevice/ipod-device-event-listener.h>


enum 
{ 
  COL_NR = 0,
  COL_TITLE,
  COL_ARTIST,
  COL_ALBUM,
  COLS
};


enum 
{ 
  COL2_NAME = 0,
  COL2_ANZAHL,
  COLS2
};


typedef struct
{
  gchar *nr;
  gchar *title;
  gchar *artist;
  gchar *album;
} TrackItem;


typedef struct
{
  gchar *name;
  gint anzahl;
} PlaylistItem;


struct _iPodVars
{
	GtkComboBox *combo;
	GtkLabel *name;
	GtkLabel *typ;
	GtkLabel *size;
	IpodDevice *device;
	Itdb_iTunesDB *it_db;
};

typedef struct _iPodVars iPodVars;


struct _iPodTrees
{
	GtkTreeView *track_tree;
	GtkTreeStore *track_store;
	GtkTreeView *playlist_tree;
	GtkTreeStore *playlist_store;
};

typedef struct _iPodTrees iPodTrees;


iPodVars vars;
iPodTrees trees;


// Prototypen
void ipod_added (IpodDeviceEventListener *listener, const gchar *udi);
void ipod_removed (IpodDeviceEventListener *listener, const gchar *udi);
void ipod_list_devices (void);
void ipod_list_device_udis (void);
gint ipod_eject_device (gchar *id);
void ipod_fill_combo (void);
void ipod_display_info (gchar *name, gchar *typ, gchar *capacity);
void setup_track_tree (void);
void setup_playlist_tree (void);
void insert_test_daten (void);



void ipod_init (void)
{
	IpodDeviceEventListener *listener;
	
	g_message ("iPod init");
	
	// Widgets holen und ablegen
	vars.combo = (GtkComboBox*) glade_xml_get_widget (glade, "combobox_ipod_list");
	vars.name = (GtkLabel*) glade_xml_get_widget (glade, "label_ipod_name");
	vars.typ = (GtkLabel*) glade_xml_get_widget (glade, "label_ipod_typ");
	vars.size = (GtkLabel*) glade_xml_get_widget (glade, "label_ipod_speicher");
	vars.device = NULL;
	vars.it_db = itdb_new ();
	
	// Listener für dynamisches erkennen von iPods
	listener = ipod_device_event_listener_new ();
	
	g_signal_connect (listener, "device-added", G_CALLBACK(ipod_added), NULL);
	g_signal_connect (listener, "device-removed", G_CALLBACK(ipod_removed), NULL);
	
	// iPod Liste befüllen
	ipod_fill_combo ();
	
	// Trees erstellen
	setup_track_tree ();
	setup_playlist_tree ();
	
	insert_test_daten ();
}

void ipod_load (void)
{
	g_message ("iPod load");
	
	//ipod_list_devices ();
}


void ipod_fill_combo (void)
{
	GtkWidget *combo;
	IpodDevice *device;
	GList *devices;
    gint i, n;
    gchar *mount_path;
	
	// iPod Liste holen
	combo = glade_xml_get_widget (glade, "combobox_ipod_list");
	if (combo == NULL) {
		g_warning ("Fehler: Konnte combobox_ipod_list nicht holen!\n");
	}
	
	// Vorhandene Einträge löschen
	gtk_combo_box_remove_text (GTK_COMBO_BOX(combo), 0);
	
	// Alle iPods finden
    devices = ipod_device_list_devices ();
    n = g_list_length (devices);
    
    // Wenn kein iPod vorhanden ist
    if (n == 0) {
        //g_warning ("No iPod devices present");
        gtk_combo_box_append_text (GTK_COMBO_BOX(combo), "Kein iPod vorhanden");
    }

	// Durch vorhandene iPods loopen
    for (i = 0; i < n; i++) {
        device = (IpodDevice *) g_list_nth_data (devices, i);
        //ipod_device_debug (device);
        g_object_get (device, "mount-point", &mount_path, NULL);
        //g_debug ("Path: %s", path);
        gtk_combo_box_append_text (GTK_COMBO_BOX(combo), mount_path);
        g_object_unref (device);
    }
    
    // Ersten iPod slektieren
    gtk_combo_box_set_active (GTK_COMBO_BOX(combo), 0);

    g_list_free (devices);
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
	Itdb_Device *dev;
	Itdb_iTunesDB *db;
    IpodDevice *device;
    GList *devices;
    gint i, n;

    devices = ipod_device_list_devices ();
    n = g_list_length (devices);
    
    if (n == 0) {
        //g_warning ("No iPod devices present");
        return;
    }

    for (i = 0; i < n; i++) {
        device = (IpodDevice *) g_list_nth_data (devices, i);
        ipod_device_debug (device);
        
        gchar *path;
        
        g_object_get (device, "mount-point", &path, NULL);
        
        g_debug ("Path: %s", path);
        
        dev = itdb_device_new ();
        db = itdb_new ();
        itdb_device_set_mountpoint (dev, path);
        
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
        //g_warning ("No iPod devices present");
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


void ipod_display_info (gchar *name, gchar *typ, gchar *capacity)
{	
	g_debug ("name: %s, typ: %s, capacity: %s", name, typ, capacity);
	
	gtk_label_set_text (GTK_LABEL (vars.name), name);
	gtk_label_set_text (GTK_LABEL (vars.typ), typ);
	gtk_label_set_text (GTK_LABEL (vars.size), capacity);
}


// Event-Handler für den Combobox change
void on_combobox_ipod_list_changed (GtkWidget *widget, gpointer user_data)
{
	gchar *mount_point, *device_name, *device_model, *capacity;
	GError *error = NULL;
	
	mount_point = gtk_combo_box_get_active_text (GTK_COMBO_BOX(widget));
	
	g_message ("iPod Liste change -> %s", mount_point);
	
	vars.device = ipod_device_new(mount_point);
	if (!vars.device) {
		g_warning ("Dies ist kein iPod!");
		return;
	}
	
	g_object_get (vars.device, "device-name", &device_name,
							   "device-model-string", &device_model,
							   "advertised-capacity", &capacity, NULL);
	
	ipod_display_info (device_name, device_model, capacity); 
	
	
	vars.it_db = itdb_parse (mount_point, &error);
	if (error) {
		g_warning (error->message);
	}
	
	guint32 track_count = itdb_tracks_number (vars.it_db);
	
	g_debug ("Track count: %i", track_count);
	
	
}


// Event-Handler für den Trennen Button
void on_button_ipod_connect_clicked (GtkWidget *widget, gpointer user_data)
{
	IpodDevice *device;
	GError *error = NULL;
	gchar *path;
	
	g_message ("iPod trennen!");
	
	path = gtk_combo_box_get_active_text (GTK_COMBO_BOX(vars.combo));
	
	device = ipod_device_new(path);
	if (!device) {
		g_warning ("Dies ist kein iPod, der getrennt werden soll!");
		return;
	}
	
	ipod_device_eject (device, &error);
	
	if (error) {
		g_warning (error->message);
	}
	
	ipod_display_info ("", "", "");
}


void setup_track_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	trees.track_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_ipod_tracks");
	if (trees.track_tree == NULL) {
		g_print ("Fehler: Konnte treeview_ipod_tracks nicht holen!\n");
	}
	
	// Nr
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
						 ("Nr", renderer, "text", COL_NR, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (trees.track_tree), column);

	// Titel
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
						 ("Titel", renderer, "text", COL_TITLE, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (trees.track_tree), column);

	// Artist
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
						 ("Artist", renderer, "text", COL_ARTIST, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (trees.track_tree), column);

	// Album
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
						 ("Album", renderer, "text", COL_ALBUM, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (trees.track_tree), column);

	// Store erstellen
	trees.track_store = gtk_tree_store_new (COLS, G_TYPE_STRING, G_TYPE_STRING,
											G_TYPE_STRING, G_TYPE_STRING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (trees.track_tree),
							 GTK_TREE_MODEL (trees.track_store));						 
}


void setup_playlist_tree (void)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	trees.playlist_tree = (GtkTreeView*) glade_xml_get_widget (glade, "treeview_ipod_playlists");
	if (trees.playlist_tree == NULL) {
		g_print ("Fehler: Konnte treeview_ipod_tracks nicht holen!\n");
	}
	
	// Name
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
						 ("Name", renderer, "text", COL2_NAME, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (trees.playlist_tree), column);

	// Anzahl
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes
						 ("Anzahl", renderer, "text", COL2_ANZAHL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (trees.playlist_tree), column);

	// Store erstellen
	trees.playlist_store = gtk_tree_store_new (COLS2, G_TYPE_STRING, G_TYPE_STRING);
											
	// Store dem Tree anhängen				
	gtk_tree_view_set_model (GTK_TREE_VIEW (trees.playlist_tree),
							 GTK_TREE_MODEL (trees.playlist_store));
}


void insert_test_daten (void)
{
	GtkTreeIter iter;
	
	gtk_tree_store_append (trees.track_store, &iter, NULL);
	gtk_tree_store_set (trees.track_store, &iter,
						COL_NR, "1", 
						COL_TITLE, "The descent",
						COL_ARTIST, "Project Pitchfork",
						COL_ALBUM, "Kaskade", -1);
						
	gtk_tree_store_append (trees.playlist_store, &iter, NULL);
	gtk_tree_store_set (trees.playlist_store, &iter,
						COL2_NAME, "Meine Lieblingstracks", 
						COL2_ANZAHL, "238", -1);
}

