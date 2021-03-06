/*
 *      lcd.h
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

#ifndef LCD_H
#define LCD_H

// Includes
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>


G_BEGIN_DECLS


#define LCD_TYPE					(lcd_get_type ())
#define LCD(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), LCD_TYPE, Lcd))
#define LCD_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), LCD_TYPE,  LcdClass))
#define IS_LCD(obj)					(G_TYPE_CHECK_INSTANCE_TYPE ((obj), LCD_TYPE))
#define IS_LCD_CLASS(klass)			(G_TYPE_CHECK_CLASS_TYPE ((klass), LCD_TYPE))


typedef struct _Lcd			Lcd;
typedef struct _LcdClass	LcdClass;

struct _Lcd {
	GtkWidget parent;
};

struct _LcdClass {
	GtkWidgetClass parent_class;
};

// Konstruktion
GType lcd_get_type (void) G_GNUC_CONST;
GtkWidget* lcd_new (void);

// Titel
void lcd_set_title (Lcd *lcd, const gchar *title);
gchar* lcd_get_title (Lcd *lcd);

// Artist
void lcd_set_artist (Lcd *lcd, const gchar *artist);
gchar* lcd_get_artist (Lcd *lcd);

// Album
void lcd_set_album (Lcd *lcd, const gchar *album);
gchar* lcd_get_album (Lcd *lcd);

// Need for Spped
void lcd_set_speed (Lcd *lcd, gint speed);
gint lcd_get_speed (Lcd *lcd);

// Need for Spped
void lcd_set_frequency (Lcd *lcd, gint frequency);
gint lcd_get_frequency (Lcd *lcd);

G_END_DECLS

#endif
