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


#include <gtk/gtk.h>


#define LCD_TYPE					(lcd_get_type ())
#define LCD(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), LCD_TYPE, Lcd))
#define LCD_CLASS(obj)				(G_TYPE_CHECK_CLASS_CAST ((obj), LCD_TYPE,  LcdClass))
#define IS_LCD(obj)					(G_TYPE_CHECK_INSTANCE_TYPE ((obj), LCD_TYPE))
#define IS_LCD_CLASS(obj)			(G_TYPE_CHECK_CLASS_TYPE ((obj), LCD_TYPE))
#define LCD_GET_CLASS				(G_TYPE_INSTANCE_GET_CLASS ((obj), LCD_TYPE, LcdClass))



typedef struct _Lcd			Lcd;
typedef struct _LcdClass	LcdClass;

struct _Lcd
{
	GtkDrawingArea parent;

	/* private */
	gchar *text;
};

struct _LcdClass
{
	GtkDrawingAreaClass parent_class;
};



GtkWidget* lcd_new (void);

void lcd_set_text (const gchar *text);









