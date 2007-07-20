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

#ifndef __LCD_H__
#define __LCD_H__


#include <gtk/gtk.h>


G_BEGIN_DECLS


#define LCD_TYPE					(lcd_get_type ())
#define LCD(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), LCD_TYPE, Lcd))
#define LCD_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST ((klass), LCD_TYPE,  LcdClass))
#define IS_LCD(obj)					(G_TYPE_CHECK_INSTANCE_TYPE ((obj), LCD_TYPE))
#define IS_LCD_CLASS(klass)			(G_TYPE_CHECK_CLASS_TYPE ((klass), LCD_TYPE))


typedef struct _Lcd			Lcd;
typedef struct _LcdClass	LcdClass;

struct _Lcd {
	GtkDrawingArea parent;
};

struct _LcdClass {
	GtkDrawingAreaClass parent_class;
};


GType lcd_get_type (void) G_GNUC_CONST;
GtkWidget* lcd_new (void);

void lcd_set_text (Lcd *lcd, const gchar *text);
gchar* lcd_get_text (Lcd *lcd);


G_END_DECLS


#endif
