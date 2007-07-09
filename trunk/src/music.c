/*
 *      music.c
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

#include "music.h"
#include "database.h"
#include "interface.h"

void music_init (void)
{
	g_message ("Music init");
	
	GtkWidget *container, *grid;
	

	// Genre
	container = glade_xml_get_widget(glade, "vbox_genre");
	grid = gnome_db_grid_new(database_get_genre_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	// Artist
	container = glade_xml_get_widget(glade, "vbox_interpret");
	grid = gnome_db_grid_new(database_get_artist_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	// Albums
	container = glade_xml_get_widget(glade, "vbox_album");
	grid = gnome_db_grid_new(database_get_album_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	// Song
	container = glade_xml_get_widget(glade, "vbox_track");
	grid = gnome_db_grid_new(database_get_song_model ());
	gtk_container_add(GTK_CONTAINER(container), grid);
	gtk_widget_show(grid);
	
	
}
