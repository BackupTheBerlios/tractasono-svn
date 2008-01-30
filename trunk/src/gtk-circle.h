/*
 *      gtk-circle.h
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

#ifndef GTK_CIRCLE_H
#define GTK_CIRCLE_H

// Includes
#include <gtk/gtk.h>
#include <cairo.h>


G_BEGIN_DECLS


#define GTK_CIRCLE_TYPE					(gtk_circle_get_type ())
#define GTK_CIRCLE(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_CIRCLE_TYPE, GtkCircle))
#define GTK_CIRCLE_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_CIRCLE_TYPE,  GtkCircleClass))
#define IS_GTK_CIRCLE(obj)				(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_CIRCLE_TYPE))
#define IS_GTK_CIRCLE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_CIRCLE_TYPE))


typedef struct _GtkCircle		GtkCircle;
typedef struct _GtkCircleClass	GtkCircleClass;

struct _GtkCircle {
	GtkWidget parent;
};

struct _GtkCircleClass {
	GtkWidgetClass parent_class;
};

// Konstruktion
GType gtk_circle_get_type (void) G_GNUC_CONST;
GtkWidget* gtk_circle_new (void);

// Need for Speed
void gtk_circle_set_speed (GtkCircle *gtk_circle, gint speed);
gint gtk_circle_get_speed (GtkCircle *gtk_circle);

G_END_DECLS

#endif
