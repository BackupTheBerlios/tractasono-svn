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



G_DEFINE_TYPE (Lcd, lcd, GTK_TYPE_DRAWING_AREA);


#define OFFSETX 2
#define OFFSETY OFFSETX
#define OFFSETW 2 * OFFSETX
#define OFFSETH 2 * OFFSETY


GtkWidget* lcd_new (void)
{
	return g_object_new (LCD_TYPE, NULL);
}


static void roundrect (cairo_t *cr, double x, double y, double w, double h, double r)
{
	cairo_move_to (cr, x + r, y);				
	cairo_line_to (cr, x + w - r, y);
	cairo_curve_to (cr, x + w, y, x + w, y, x + w, y + r);
	cairo_line_to (cr, x + w, y + h - r);
	cairo_curve_to (cr, x + w, y + h, x + w, y + h, x + w - r, y + h);
	cairo_line_to (cr, x + r, y + h);
	cairo_curve_to (cr, x, y + h, x, y + h, x, y + h - r);
	cairo_line_to (cr, x, y + r);
	cairo_curve_to (cr, x, y, x, y, x + r, y);
}


static void draw_message (cairo_t *cr, double w, double h, const gchar *text)
{
	cairo_text_extents_t extents;

	gdouble x = w / 2;
	gdouble y = h / 2;

	cairo_select_font_face (cr, "Mono",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);

	cairo_set_font_size (cr, 24);
	cairo_text_extents (cr, text, &extents);
	x = x - (extents.width / 2 + extents.x_bearing);
	y = y - (extents.height / 2 + extents.y_bearing);

	cairo_move_to (cr, x, y);
	cairo_show_text (cr, text);
}


static void draw (GtkWidget *lcd, cairo_t *cr)
{
	double x, y, w, h;

	// Koordinaten setzen
	x = OFFSETX;
	y = OFFSETY;
	w = lcd->allocation.width - OFFSETW;
	h = lcd->allocation.height - OFFSETH;
	
	g_debug ("LCD Widget Koordinaten: x=%.0f y=%.0f w=%.0f h=%.0f", x, y, w, h);
	
	// Antialias setzen
	//cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL);
	
	// Liniendicke setzen
	//cairo_set_line_width (cr, 0.04);
	
	// Rahmen zeichnen
	//cairo_rectangle (cr, x, y, w, h);
	roundrect (cr, x, y, w, h, 12);
	
	// Farben setzen
	cairo_set_source_rgb (cr, 0.7, 0.8, 0.8);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	
	// Text zeichnen
	draw_message (cr, w, h, "tractasono");
	
	// Alles zeichnen
	cairo_stroke (cr);
}


static gboolean lcd_expose (GtkWidget *lcd, GdkEventExpose *event)
{
	cairo_t *cr;

	/* get a cairo_t */
	cr = gdk_cairo_create (lcd->window);

	/* set a clip region for the expose event */
	cairo_rectangle (cr, event->area.x, event->area.y,
					 event->area.width, event->area.height);
	cairo_clip (cr);

	draw (lcd, cr);

	cairo_destroy (cr);

	return FALSE;
}


static void lcd_class_init (LcdClass *class)
{
	GtkWidgetClass *widget_class;

	widget_class = GTK_WIDGET_CLASS (class);

	widget_class->expose_event = lcd_expose;
}


static void lcd_init (Lcd *lcd)
{
	g_debug ("lcd_init");
}


void lcd_set_text (const gchar *text)
{
	g_message ("Die Funktion lcd_set_text ist noch nicht implementiert!");
}




