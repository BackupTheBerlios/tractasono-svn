/*
 *      disc.h
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

#ifndef DISC_H
#define DISC_H

#include <gtk/gtk.h>
#include <glade/glade.h>



void disc_init(void);

void disc_reread (void);

void on_extract_toggled (GtkCellRendererToggle *cellrenderertoggle,
                                gchar *path,
                                gpointer user_data);


void number_cell_icon_data_cb (GtkTreeViewColumn *tree_column,
							   GtkCellRenderer *cell,
							   GtkTreeModel *tree_model,
							   GtkTreeIter *iter,
							   gpointer data);
/*						   		   
void on_cell_edited (GtkCellRendererText *renderer, gchar *path,
					 gchar *string, gpointer column_data);
*/             
            
void duration_cell_data_cb (GtkTreeViewColumn *tree_column,
							GtkCellRenderer *cell,
							GtkTreeModel *tree_model,
							GtkTreeIter *iter,
							gpointer data);



#endif
