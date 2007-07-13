/*
 *      lcd.c
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

#include <math.h>

#include "lcd.h"


#define OFFSETX 6
#define OFFSETY 10


G_DEFINE_TYPE (EggClockFace, egg_clock_face, GTK_TYPE_DRAWING_AREA);





GtkWidget* egg_clock_face_new (void)
{
	return g_object_new (EGG_TYPE_CLOCK_FACE, NULL);
}


static void roundedrec (cairo_t *cr, double x, double y, double w, double h, double r)
{
	cairo_move_to (cr, x + r, y);				
	cairo_line_to (cr, x + w - r, y);
	cairo_curve_to (cr, x + w, y, x + w, y, x + w, y + r);
	cairo_line_to (cr, x + w, y + h - r);
	cairo_curve_to (cr, x+w,y+h,x+w,y+h,x+w-r,y+h);
	cairo_line_to (cr, x+r,y+h);
	cairo_curve_to (cr, x,y+h,x,y+h,x,y+h-r);
	cairo_line_to (cr, x,y+r);
	cairo_curve_to (cr, x,y,x,y,x+r,y);
	return;
}
    

static void draw (GtkWidget *clock, cairo_t *cr)
{
	double x, y, width, height;

	x = clock->allocation.x + OFFSETX;
	y = clock->allocation.y + OFFSETY;
	width = clock->allocation.width - 2*OFFSETX;
	height = clock->allocation.height - 2*OFFSETY;
	
	//cairo_rectangle (cr, x, y, width, height);
	roundedrec (cr, x, y, width, height, 15);

	/* clock back */
	cairo_set_source_rgb (cr, 0.7, 0.8, 0.8);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);
}


static gboolean egg_clock_face_expose (GtkWidget *clock, GdkEventExpose *event)
{
	cairo_t *cr;

	/* get a cairo_t */
	cr = gdk_cairo_create (clock->window);

	/* set a clip region for the expose event */
	cairo_rectangle (cr, event->area.x, event->area.y,
					 event->area.width, event->area.height);
	cairo_clip (cr);

	draw (clock, cr);

	cairo_destroy (cr);

	return FALSE;
}


static void egg_clock_face_class_init (EggClockFaceClass *class)
{
	GtkWidgetClass *widget_class;

	widget_class = GTK_WIDGET_CLASS (class);

	widget_class->expose_event = egg_clock_face_expose;
}


static void egg_clock_face_init (EggClockFace *clock)
{
	g_debug ("egg_clock_face_init");
}






