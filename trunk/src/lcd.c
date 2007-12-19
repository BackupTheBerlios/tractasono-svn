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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <math.h>
#include <string.h>
#include "lcd.h"


#define LCD_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), LCD_TYPE, LcdPrivate))

#define LCD_TIMEOUT_MS 1000
#define LCD_SPEED_PX 50

#define OFFSETX 1
#define OFFSETY OFFSETX
#define OFFSETW 2 * OFFSETX
#define OFFSETH 2 * OFFSETY


typedef struct _LcdPrivate LcdPrivate;

struct _LcdPrivate {
	gchar *title;
	gchar *artist;
	gchar *album;
	gchar *uri;
	gchar *duration;
	gint speed;
	gint current_x;
};

enum {
	PROP_0,
	PROP_TITLE,
	PROP_ARTIST,
	PROP_ALBUM,
	PROP_URI,
	PROP_DURATION
};


// Prototypen
static void lcd_class_init (LcdClass *klass);
static void lcd_init (Lcd *lcd);
static void lcd_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void lcd_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static gboolean lcd_expose (GtkWidget *lcd, GdkEventExpose *event);
static gboolean lcd_redraw_canvas (gpointer data);
static gchar* construct_message (Lcd *lcd);

GType lcd_get_type (void)
{
	static GType lcd_type = 0;
	
	if (!lcd_type) {
		static const GTypeInfo lcd_info =
		{
			sizeof (LcdClass),
			NULL,
			NULL,
			(GClassInitFunc) lcd_class_init,
			NULL,
			NULL,
			sizeof (Lcd),
			0,
			(GInstanceInitFunc) lcd_init,
		};
		
		lcd_type = g_type_register_static (GTK_TYPE_DRAWING_AREA, "Lcd", &lcd_info, 0);
	}
	
	return lcd_type;
}


static void lcd_class_init (LcdClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	
	/* Override the standard functions for setting and retrieving properties. */
	gobject_class->set_property = lcd_set_property;
	gobject_class->get_property = lcd_get_property;
	
	/* Override the standard functions for drawing the widget. */
	widget_class->expose_event = lcd_expose;

	/* Add LcdPrivate as a private data class of LcdClass. */
	g_type_class_add_private (klass, sizeof (LcdPrivate));

	/* Register one GObject property for the title */
	g_object_class_install_property (gobject_class, PROP_TITLE,
									 g_param_spec_pointer ("text", "The text",
									 					   "The text to show in the lcd",
									 					   G_PARAM_READWRITE));
	/* Register one GObject property for the artist */
	g_object_class_install_property (gobject_class, PROP_ARTIST,
									 g_param_spec_pointer ("artist", "The artist",
									 					   "The artist to show in the lcd",
									 					   G_PARAM_READWRITE));	
	/* Register one GObject property for the album */
	g_object_class_install_property (gobject_class, PROP_ALBUM,
									 g_param_spec_pointer ("album", "The album",
									 					   "The album to show in the lcd",
									 					   G_PARAM_READWRITE));
	/* Register one GObject property for the uri */
	g_object_class_install_property (gobject_class, PROP_URI,
									 g_param_spec_pointer ("uri", "The uri",
									 					   "The uri to show in the lcd",
									 					   G_PARAM_READWRITE));
	/* Register one GObject property for the duration */
	g_object_class_install_property (gobject_class, PROP_DURATION,
									 g_param_spec_pointer ("duration", "The duration",
									 					   "The duration to show in the lcd",
									 					   G_PARAM_READWRITE));	
}


/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void lcd_set_property (GObject *object,
                              guint prop_id,
                              const GValue *value,
                              GParamSpec *pspec)
{
	Lcd *lcd = LCD(object);

	switch (prop_id) {
		case PROP_TITLE:
			lcd_set_title (lcd, g_value_get_string (value));
			break;
		case PROP_ARTIST:
			lcd_set_artist (lcd, g_value_get_string (value));
			break;
		case PROP_ALBUM:
			lcd_set_album (lcd, g_value_get_string (value));
			break;
		case PROP_URI:
			lcd_set_uri (lcd, g_value_get_string (value));
			break;
		case PROP_DURATION:
			lcd_set_duration (lcd, g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


/* This function is called when the programmer requests the value of a widget
 * property with g_object_get(). */
static void lcd_get_property (GObject *object,
                         	  guint prop_id,
                         	  GValue *value,
                      		  GParamSpec *pspec)
{
	Lcd *lcd = LCD (object);
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	switch (prop_id)
	{
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_ARTIST:
			g_value_set_string (value, priv->artist);
			break;
		case PROP_ALBUM:
			g_value_set_string (value, priv->album);
			break;
		case PROP_URI:
			g_value_set_string (value, priv->uri);
			break;
		case PROP_DURATION:
			g_value_set_string (value, priv->duration);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


GtkWidget* lcd_new (void)
{
	return g_object_new (LCD_TYPE, NULL);
}


// Rundes Rechteck zeichnen
static void rectangle_round (cairo_t *cr, double x, double y, double w, double h, double r)
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


// Den Text zeichnen
static void draw_message (cairo_t *cr, Lcd *lcd, double w, double h, const gchar *message)
{
	LcdPrivate *priv;
	GtkWidget *widget;
	
	cairo_text_extents_t extents;

	priv = LCD_GET_PRIVATE (lcd);
	widget = GTK_WIDGET (lcd);

	cairo_select_font_face (cr, "Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 24);
	cairo_text_extents (cr, message, &extents);

	//cairo_move_to (cr, priv->current_x, (widget->allocation.height - (extents.height)) / 2);
	
	g_debug ("höhe text: %f | höhe platz: %d | de bär: %f", extents.height, widget->allocation.height, extents.y_bearing);
	
	gdouble x, y;
	
	x = priv->current_x;
	//y = (widget->allocation.height / 2) + (extents.height / 2);
	y = (widget->allocation.height / 2) + (extents.height / 2) - (extents.height + extents.y_bearing);
	
	g_debug ("text move to: x=%f y=%f", x, y);
	
	cairo_move_to (cr, x, y);
	cairo_show_text (cr, message);
	
}


static void draw (GtkWidget *lcd, cairo_t *cr)
{
	double x, y, w, h;
	gchar *message;

	// Koordinaten setzen
	x = OFFSETX;
	y = OFFSETY;
	w = lcd->allocation.width - OFFSETW;
	h = lcd->allocation.height - OFFSETH;
	
	//g_debug ("LCD Widget Koordinaten: x=%.0f y=%.0f w=%.0f h=%.0f", x, y, w, h);
	
	// Antialias setzen
	//cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL);
	
	// Liniendicke setzen
	cairo_set_line_width (cr, 2);
	
	// Rahmen zeichnen
	cairo_rectangle (cr, x, y, w, h);
	//rectangle_round (cr, x, y, w, h, 12);
	
	// Farben setzen
	cairo_set_source_rgb (cr, 0.7, 0.8, 0.8);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	
	// Text zeichnen
	message = construct_message (LCD (lcd));
	draw_message (cr, LCD (lcd), w, h, message);
	
	// Alles zeichnen
	cairo_stroke (cr);
}


static gboolean lcd_expose (GtkWidget *lcd, GdkEventExpose *event)
{
	cairo_t *cr;
	
	g_debug ("expose event");

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


static void lcd_init (Lcd *lcd)
{
	g_message ("\tLCD Widget init");
	
	LcdPrivate *priv;
	priv = LCD_GET_PRIVATE (lcd);
	
	priv->current_x = 0;
	priv->speed = LCD_SPEED_PX;
	
	/* update the LCD */
	g_timeout_add (LCD_TIMEOUT_MS, (GSourceFunc) lcd_slide, (gpointer) lcd);
}


// Erzeugt den Message String aus den abgefüllten Members
static gchar* construct_message (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	GString *message;
	
	message = g_string_new (NULL);

	if (!priv->artist && !priv->title) {
		if (priv->uri) {
			g_string_append (message, priv->uri);	
		} else {
			g_string_append (message, "...");
		}
	} else if (!priv->artist) {
		g_string_append (message, priv->title);
	} else if (!priv->title) {
		g_string_append (message, priv->artist);
	} else {
		g_string_append (message, priv->artist);
		g_string_append (message, " - ");
		g_string_append (message, priv->title);
	}
	
	return message->str;
}


void lcd_set_title (Lcd *lcd, const gchar *title)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	if (title != NULL) {
		// Update Members
		priv->title = strdup (title);
		// LCD updaten
		lcd_redraw_canvas (lcd);
	} else {
		priv->title = NULL;
	}

}


void lcd_set_artist (Lcd *lcd, const gchar *artist)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	if (artist != NULL) {
		// Update Members
		priv->artist = strdup (artist);
		// LCD updaten
		lcd_redraw_canvas (lcd);
	} else {
		priv->artist = NULL;
	}
}


void lcd_set_album (Lcd *lcd, const gchar *album)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	if (album != NULL) {
		// Update Members
		priv->album = strdup (album);
		// LCD updaten
		lcd_redraw_canvas (lcd);
	} else {
		priv->album = NULL;
	}
}


void lcd_set_uri (Lcd *lcd, const gchar *uri)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	
	if (uri != NULL) {
		// Update Members
		priv->uri = strdup (uri);
		// LCD updaten
		lcd_redraw_canvas (lcd);
	} else {
		priv->uri = NULL;
	}
}


void lcd_set_duration (Lcd *lcd, const gchar *duration)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	// Update Members
	priv->duration = strdup (duration);
	
	// LCD updaten
	lcd_redraw_canvas (lcd);
}


gchar* lcd_get_title (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return priv->title;
}


gchar* lcd_get_artist (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return priv->artist;
}


gchar* lcd_get_album (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return priv->album;
}


gchar* lcd_get_uri (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return priv->uri;
}


gchar* lcd_get_duration (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return priv->duration;
}


static gboolean lcd_redraw_canvas (gpointer data)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (data);
	if (!widget->window) {
		return FALSE;
	}

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);

	gdk_region_destroy (region);
	
	return TRUE; /* keep running this event */
}


void lcd_set_speed (Lcd *lcd, gint speed)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	priv->speed = speed;
}


gint lcd_get_speed (Lcd *lcd)
{
	return LCD_GET_PRIVATE (lcd)->speed;
}


void lcd_slide (Lcd *lcd)
{
	LcdPrivate *priv;
	GtkWidget *widget;
	gchar *message;
	
	cairo_t *cr;
	cairo_text_extents_t extents;

	priv = LCD_GET_PRIVATE (lcd);
	widget = GTK_WIDGET (lcd);
	
	/* get a cairo_t */
	cr = gdk_cairo_create (widget->window);
	

	message = construct_message (LCD (lcd));

	cairo_select_font_face (cr, "Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 24);
	cairo_text_extents (cr, message, &extents);
	
	g_debug ("width text: %f | width platz: %d", extents.width, widget->allocation.width);
	
	// Nur sliden wenn Text grösser als Platz ist
	if (extents.width > widget->allocation.width) {
		/* Scroll the message "speed" pixels to the left or wrap around. */
		priv->current_x = priv->current_x - priv->speed;
		
		if ((priv->current_x + (extents.width)) <= 0) {
			priv->current_x = widget->allocation.width;
		}
	} else {
		priv->current_x = (widget->allocation.width / 2) - (extents.width / 2);
	}
	
	
	draw (widget, cr);

	cairo_destroy (cr);

}

