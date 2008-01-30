/*
 *      gtk-circle.c
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

#include "gtk-circle.h"


#define GTK_CIRCLE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_CIRCLE_TYPE, GtkCirclePrivate))


typedef struct _GtkCirclePrivate GtkCirclePrivate;

struct _GtkCirclePrivate {
	gint speed;
};

enum {
	PROP_0,
	PROP_SPEED
};


// Prototypen
static void gtk_circle_class_init (GtkCircleClass *klass);
static void gtk_circle_init (GtkCircle *gtk_circle);
static void gtk_circle_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gtk_circle_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void gtk_circle_realize (GtkWidget *widget);
static gboolean gtk_circle_expose (GtkWidget *widget, GdkEventExpose *event);
static gboolean gtk_circle_redraw_canvas (GtkCircle *gtk_circle);


// Gibt den Typ zurück
GType gtk_circle_get_type (void)
{
	static GType gtk_circle_type = 0;
	
	if (!gtk_circle_type) {
		static const GTypeInfo gtk_circle_info =
		{
			sizeof (GtkCircleClass),
			NULL,
			NULL,
			(GClassInitFunc) gtk_circle_class_init,
			NULL,
			NULL,
			sizeof (GtkCircle),
			0,
			(GInstanceInitFunc) gtk_circle_init,
		};
		
		gtk_circle_type = g_type_register_static (GTK_TYPE_WIDGET, "GtkCircle", &gtk_circle_info, 0);
	}
	
	return gtk_circle_type;
}


// Klassen Konstruktor
static void gtk_circle_class_init (GtkCircleClass *klass)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;
	
	gobject_class = G_OBJECT_CLASS (klass);
	widget_class = GTK_WIDGET_CLASS (klass);
	
	/* Override the standard functions for setting and retrieving properties. */
	gobject_class->set_property = gtk_circle_set_property;
	gobject_class->get_property = gtk_circle_get_property;
	
	/* Override the standard functions for drawing the widget. */
	widget_class->realize = gtk_circle_realize;
	widget_class->expose_event = gtk_circle_expose;

	/* Add GtkCirclePrivate as a private data class of GtkCircleClass. */
	g_type_class_add_private (klass, sizeof (GtkCirclePrivate));
   
	/* Register one GObject property for the speed */
	g_object_class_install_property (gobject_class, PROP_SPEED,
									 g_param_spec_int ("speed", "Speed of the rotation",
													   "Rotations per Second",
													   1, 100, 50, 
													   G_PARAM_READWRITE));
}


/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void gtk_circle_set_property (GObject *object,
                              guint prop_id,
                              const GValue *value,
                              GParamSpec *pspec)
{
	GtkCircle *gtk_circle = GTK_CIRCLE (object);

	switch (prop_id) {
		case PROP_SPEED:
			gtk_circle_set_speed (gtk_circle, g_value_get_int (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


/* This function is called when the programmer requests the value of a widget
 * property with g_object_get(). */
static void gtk_circle_get_property (GObject *object,
                         	  guint prop_id,
                         	  GValue *value,
                      		  GParamSpec *pspec)
{
	GtkCircle *gtk_circle = GTK_CIRCLE (object);
	GtkCirclePrivate *priv = GTK_CIRCLE_GET_PRIVATE (gtk_circle);

	switch (prop_id)
	{
		case PROP_SPEED:
			g_value_set_int (value, priv->speed);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


GtkWidget* gtk_circle_new (void)
{
	return GTK_WIDGET (g_object_new (gtk_circle_get_type (), NULL));
}


/* Called when the widget is realized. This usually happens when you call
 * gtk_widget_show() on the widget. */
static void gtk_circle_realize (GtkWidget *widget)
{
	GtkCircle *gtk_circle;
	GdkWindowAttr attributes;
	gint attr_mask;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_GTK_CIRCLE (widget));

	/* Set the GTK_REALIZED flag so it is marked as realized. */
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	gtk_circle = GTK_CIRCLE (widget);

	/* Create a new GtkWindowAttr object that will hold info about the GdkWindow. */
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events (widget);
	attributes.event_mask |= (GDK_EXPOSURE_MASK);
	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	/* Create a new GdkWindow for the widget. */
	attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
	widget->window = gdk_window_new (widget->parent->window, &attributes, attr_mask);
	gdk_window_set_user_data (widget->window, gtk_circle);

	/* Attach a style to the GdkWindow and draw a background color. */
	widget->style = gtk_style_attach (widget->style, widget->window);
	gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
	gdk_window_show (widget->window);
}



static gboolean gtk_circle_expose (GtkWidget *widget, GdkEventExpose *event)
{
	GtkCircle *gtk_circle;
	cairo_t *cr;
	
	//g_debug ("expose event");

	gtk_circle = GTK_CIRCLE (widget);

	/* get a cairo_t */
	cr = gdk_cairo_create (widget->window);

	/* set a clip region for the expose event */
	cairo_rectangle (cr, event->area.x, event->area.y,
					 event->area.width, event->area.height);
	cairo_clip (cr);

	// Hier zeichnen!!!

	// free cairo_t *
	cairo_destroy (cr);

	return FALSE;
}


static void gtk_circle_init (GtkCircle *gtk_circle)
{
	g_message ("GtkCircle Widget init");
	
	GtkCirclePrivate *priv;
	priv = GTK_CIRCLE_GET_PRIVATE (gtk_circle);
	
	// default speed
	priv->speed = 50;
}


void gtk_circle_set_speed (GtkCircle *gtk_circle, gint speed)
{
	GtkCirclePrivate *priv = GTK_CIRCLE_GET_PRIVATE (gtk_circle);
	
	priv->speed = speed;
	gtk_circle_redraw_canvas (gtk_circle);
}


gint gtk_circle_get_speed (GtkCircle *gtk_circle)
{
	GtkCirclePrivate *priv = GTK_CIRCLE_GET_PRIVATE (gtk_circle);
	
	return priv->speed;
}


static gboolean gtk_circle_redraw_canvas (GtkCircle *gtk_circle)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (gtk_circle);
	if (!widget->window) {
		return FALSE;
	}

	//g_debug ("gtk_circle_redraw_canvas start");

	region = gdk_drawable_get_clip_region (widget->window);
	
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);
	
	//g_debug ("gtk_circle_redraw_canvas stop");

	gdk_region_destroy (region);
	
	return TRUE; /* keep running this event */
}

