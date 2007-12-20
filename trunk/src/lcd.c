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

#include "lcd.h"


// Defines
#define FRAME_ABSTAND 10


#define LCD_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), LCD_TYPE, LcdPrivate))


typedef struct _LcdPrivate LcdPrivate;

struct _LcdPrivate {
	gchar *title;
	gchar *artist;
	gchar *album;
	gint speed;
	gint frequency;
	gboolean frequency_switch; // helper variable
	gdouble current_x;
};

enum {
	PROP_0,
	PROP_TITLE,
	PROP_ARTIST,
	PROP_ALBUM,
	PROP_SPEED,
	PROP_FREQUENCY
};


// Prototypen
static void lcd_class_init (LcdClass *klass);
static void lcd_init (Lcd *lcd);
static void lcd_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void lcd_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void lcd_realize (GtkWidget *widget);
static gboolean lcd_expose (GtkWidget *widget, GdkEventExpose *event);
static gboolean lcd_redraw_canvas (Lcd *lcd);
static gchar* construct_text (Lcd *lcd);
gboolean lcd_scroll (Lcd *);
static void set_timer (gpointer data);


// Gibt den Typ zurück
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
		
		lcd_type = g_type_register_static (GTK_TYPE_WIDGET, "Lcd", &lcd_info, 0);
	}
	
	return lcd_type;
}


// Klassen Konstruktor
static void lcd_class_init (LcdClass *klass)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;
	
	gobject_class = G_OBJECT_CLASS (klass);
	widget_class = GTK_WIDGET_CLASS (klass);
	
	/* Override the standard functions for setting and retrieving properties. */
	gobject_class->set_property = lcd_set_property;
	gobject_class->get_property = lcd_get_property;
	
	/* Override the standard functions for drawing the widget. */
	widget_class->realize = lcd_realize;
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
									 					   
	/* Register one GObject property for the speed */
	g_object_class_install_property (gobject_class, PROP_SPEED,
									 g_param_spec_int ("speed", "Speed of the scroll",
													   "The pixels of movement every second",
													   1, 100, 50, 
													   G_PARAM_READWRITE));
	/* Register one GObject property for the frequency */
	g_object_class_install_property (gobject_class, PROP_FREQUENCY,
									 g_param_spec_int ("frequency", "Frequency of the scroll",
													   "The frequency of scrolling in milliseconds",
													   1, 2000, 1000, 
													   G_PARAM_READWRITE));
}


/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void lcd_set_property (GObject *object,
                              guint prop_id,
                              const GValue *value,
                              GParamSpec *pspec)
{
	Lcd *lcd = LCD (object);

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
		case PROP_SPEED:
			lcd_set_speed (lcd, g_value_get_int (value));
			break;
		case PROP_FREQUENCY:
			lcd_set_frequency (lcd, g_value_get_int (value));
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
		case PROP_SPEED:
			g_value_set_int (value, priv->speed);
			break;
		case PROP_FREQUENCY:
			g_value_set_int (value, priv->frequency);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}


GtkWidget* lcd_new (void)
{
	return GTK_WIDGET (g_object_new (lcd_get_type (), NULL));
}


// Den Text zeichnen
static void draw_text (Lcd *lcd, cairo_t *cr)
{
	LcdPrivate *priv;
	GtkWidget *widget;
	cairo_text_extents_t extents;
	gdouble x, y;
	gchar *text;

	priv = LCD_GET_PRIVATE (lcd);
	widget = GTK_WIDGET (lcd);
	
	// Text erstellen
	text = construct_text (LCD (lcd));
	
	// Farbe setzen
	cairo_set_source_rgb (cr, 0, 0, 0);

	// Font setzen
	cairo_select_font_face (cr, "Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 24);
	cairo_text_extents (cr, text, &extents);
	
	
	// Nur sliden wenn Text grösser als Platz ist
	if (extents.width < widget->allocation.width - FRAME_ABSTAND) {
		// Text horizontal zentrieren
		priv->current_x = (widget->allocation.width / 2) - (extents.width / 2);
	} else if ((priv->current_x + extents.width) <= 0) {
		// falls Text zu Ende -> wieder hinten anfangen
		priv->current_x = widget->allocation.width;
	}
	
	x = priv->current_x;
	y = (widget->allocation.height / 2) + (extents.height / 2) - (extents.height + extents.y_bearing);	
	
	//g_debug ("text position: x=%.1f y=%.1f", x, y);
	
	cairo_move_to (cr, x, y);
	cairo_show_text (cr, text);
}


static void draw_frame (Lcd *lcd, cairo_t *cr)
{
	double x, y, w, h;
	GtkWidget *widget;

	widget = GTK_WIDGET (lcd);

	// Koordinaten setzen
	x = 0;
	y = 0;
	w = widget->allocation.width;
	h = widget->allocation.height;
	
	//g_debug ("LCD frame position: x=%.1f y=%.1f w=%.1f h=%.1f", x, y, w, h);
	
	// Liniendicke setzen
	cairo_set_line_width (cr, 2);
	
	// Rahmen zeichnen
	cairo_rectangle (cr, x, y, w, h);
	
	// Farben setzen
	cairo_set_source_rgb (cr, 0.7, 0.8, 0.8);
	cairo_fill_preserve (cr);
}



/* Called when the widget is realized. This usually happens when you call
 * gtk_widget_show() on the widget. */
static void lcd_realize (GtkWidget *widget)
{
	Lcd *lcd;
	GdkWindowAttr attributes;
	gint attr_mask;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_LCD (widget));

	/* Set the GTK_REALIZED flag so it is marked as realized. */
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	lcd = LCD (widget);

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
	gdk_window_set_user_data (widget->window, lcd);

	/* Attach a style to the GdkWindow and draw a background color. */
	widget->style = gtk_style_attach (widget->style, widget->window);
	gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
	gdk_window_show (widget->window);
}



static gboolean lcd_expose (GtkWidget *widget, GdkEventExpose *event)
{
	Lcd *lcd;
	cairo_t *cr;
	
	//g_debug ("expose event");

	lcd = LCD (widget);

	/* get a cairo_t */
	cr = gdk_cairo_create (widget->window);

	/* set a clip region for the expose event */
	cairo_rectangle (cr, event->area.x, event->area.y,
					 event->area.width, event->area.height);
	cairo_clip (cr);

	draw_frame (lcd, cr);
	draw_text (lcd, cr);
	
	// Alles zeichnen
	cairo_stroke (cr);

	// free cairo_t *
	cairo_destroy (cr);

	return FALSE;
}


static void lcd_init (Lcd *lcd)
{
	g_message ("\tLCD Widget init");
	
	LcdPrivate *priv;
	priv = LCD_GET_PRIVATE (lcd);
	
	priv->current_x = 0;
	
	// 50 pixels
	priv->speed = 50;
	
	// 1000 milliseconds
	priv->frequency = 1000;
	
	/* set frequency timer */
	set_timer ((gpointer) lcd);
}


// Erzeugt den Text aus den abgefüllten Members
static gchar* construct_text (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	gchar *text;
	
	if (!priv->artist && !priv->title) {
		text = g_strdup ("");
	} else if (!priv->artist) {
		text = g_strdup (priv->title);
	} else if (!priv->title) {
		text = g_strdup (priv->artist);
	} else {
		text = g_strdup_printf ("%s - %s", priv->artist, priv->title);
	}
	
	return text;
}


void lcd_set_title (Lcd *lcd, const gchar *title)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	if (priv->title) {
		g_free (priv->title);
		priv->title = NULL;
	}
	priv->title = g_strdup (title);
	lcd_redraw_canvas (lcd);
}


void lcd_set_artist (Lcd *lcd, const gchar *artist)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	if (priv->artist) {
		g_free (priv->artist);
		priv->artist = NULL;
	}
	priv->artist = g_strdup (artist);
	lcd_redraw_canvas (lcd);
}


void lcd_set_album (Lcd *lcd, const gchar *album)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);

	if (priv->album) {
		g_free (priv->album);
		priv->album = NULL;
	}
	priv->album = g_strdup (album);
	lcd_redraw_canvas (lcd);
}


void lcd_set_speed (Lcd *lcd, gint speed)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	
	priv->speed = speed;
	lcd_redraw_canvas (lcd);
}



void lcd_set_frequency (Lcd *lcd, gint frequency)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	
	priv->frequency = frequency;
	priv->frequency_switch = TRUE;
	
	lcd_redraw_canvas (lcd);
}



gchar* lcd_get_title (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return g_strdup (priv->title);
}


gchar* lcd_get_artist (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return g_strdup (priv->artist);
}


gchar* lcd_get_album (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	return g_strdup (priv->album);
}


gint lcd_get_speed (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	
	return priv->speed;
}


gint lcd_get_frequency (Lcd *lcd)
{
	LcdPrivate *priv = LCD_GET_PRIVATE (lcd);
	
	return priv->frequency;
}



static gboolean lcd_redraw_canvas (Lcd *lcd)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (lcd);
	if (!widget->window) {
		return FALSE;
	}

	//g_debug ("lcd_redraw_canvas start");

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);
	
	//g_debug ("lcd_redraw_canvas stop");

	gdk_region_destroy (region);
	
	return TRUE; /* keep running this event */
}



gboolean lcd_scroll (Lcd *lcd)
{
	LcdPrivate *priv;

	priv = LCD_GET_PRIVATE (lcd);
	priv->current_x = priv->current_x - priv->speed;
		
	lcd_redraw_canvas (lcd);
	
	if (priv->frequency_switch) {
		return FALSE;
	}
	
	return TRUE;
}


static void set_timer (gpointer data)
{
	Lcd *lcd;
	LcdPrivate *priv;

	lcd = LCD (data);
	priv = LCD_GET_PRIVATE (lcd);
	
	//g_debug ("set_timer (%d milliseconds)", priv->frequency);
	priv->frequency_switch = FALSE;
	
	g_timeout_add_full (G_PRIORITY_DEFAULT, priv->frequency,
						(GSourceFunc) lcd_scroll, data,
						(GDestroyNotify) set_timer);
}
