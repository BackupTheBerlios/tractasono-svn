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
#include <math.h>


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
static void gtk_circle_paint (GtkCircle *gtk_circle);
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
	
	//g_debug ("expose event");

	gtk_circle = GTK_CIRCLE (widget);

	// Hier zeichnen!!!
	gtk_circle_paint(gtk_circle);


	return FALSE;
}


static void gtk_circle_paint (GtkCircle *gtk_circle)
{
	cairo_t*  cr = NULL;
	gdouble   fWidth   = (gdouble) GTK_WIDGET(gtk_circle)->allocation.width;
	gdouble   fHeight  = (gdouble) GTK_WIDGET(gtk_circle)->allocation.height;
//	gdouble   fSeconds;
//	gulong    ulMicroSeconds;


	/* deal with the timing stuff 
	fSeconds = g_timer_elapsed (pPassAround->pTimer, &ulMicroSeconds);
	g_message("timer: %f", fSeconds);
	*/
	/*
	if (fSeconds >= 4.0f)
		g_timer_reset (pPassAround->pTimer);
	*/
	/* clear drawing-context */
	cr = gdk_cairo_create (GTK_WIDGET(gtk_circle)->window);
	
	/* set the clipping-rectangle */
	cairo_rectangle (cr, 0.0f, 0.0f, fWidth, fHeight);
	cairo_clip (cr);

	gint i;
	gdouble angle1;
	gdouble angle2;
	gdouble x, y;
	gdouble pos_x, pos_y;
	gdouble kreisgroesse;
	gdouble punktegroesse;

	x = fWidth / 2.0f;
	y = fHeight / 2.0f;
	if (fWidth < fHeight){
		kreisgroesse = fWidth / 2.0f * 0.6; 	
	} else {
		kreisgroesse = fHeight / 2.0f * 0.6;
	}
	punktegroesse = kreisgroesse / 4.5;
	// g_message ("kreisgroesse : %f", kreisgroesse);
	static gint aktiverpunkt = 0;
	if (aktiverpunkt >= 315) {
		aktiverpunkt = 0;
	} else {
		aktiverpunkt = aktiverpunkt + 45;
	} 
	
	// gint drehung_max = drehung + 315;

	for (i = aktiverpunkt; i <= aktiverpunkt + 315; i = i + 45) {
		/*
		g_message ("i : %d", i);
		g_message ("aktiverpunkt : %d", aktiverpunkt);
		*/
		angle1 = i * (M_PI / 180.0);
		angle2 = angle1 + (45 * (M_PI / 180.0));

		cairo_set_line_width (cr, 0.0);
		cairo_arc (cr, x, y, kreisgroesse, angle1, angle2);
		
		cairo_get_current_point (cr, &pos_x, &pos_y);

		cairo_stroke (cr);

		//g_message ("x : %f, y : %f  /  pos_x : %f, pos_y : %f", x, y, pos_x, pos_y);

		cairo_set_line_width (cr, 2.0);
		cairo_arc (cr, pos_x, pos_y, punktegroesse, 0, 2 * M_PI);
		if(aktiverpunkt == i) {
			cairo_set_source_rgba (cr, 0.074509804, 0.074509804, 0.905882353, 1.0f);
		} else {
			cairo_set_source_rgba (cr, 0.074509804, 0.074509804, 0.905882353, 0.3); // 0.792156863, 0.792156863, 1
		}
		
		cairo_fill_preserve (cr);
		if(aktiverpunkt == i) {
			cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0f);
		} else {
			cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.6);
		}
		cairo_stroke(cr);
		
	}
	
	cairo_destroy (cr);
}

gboolean redraw_handler (gpointer data)
{
	g_message("redraw handler aufgerufen...");
	
	if(IS_GTK_CIRCLE(data)) {
		gtk_widget_queue_draw (GTK_WIDGET(data));
		return TRUE;
	} else {
		return FALSE;
	}
}


static void gtk_circle_init (GtkCircle *gtk_circle)
{
	g_message ("GtkCircle Widget init");
	
	GtkCirclePrivate *priv;
	priv = GTK_CIRCLE_GET_PRIVATE (gtk_circle);
	
	g_timeout_add (250, redraw_handler, (gpointer) gtk_circle);
	
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

