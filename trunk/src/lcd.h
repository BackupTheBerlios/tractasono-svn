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


#define EGG_TYPE_CLOCK_FACE				(egg_clock_face_get_type ())
#define EGG_CLOCK_FACE(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), EGG_TYPE_CLOCK_FACE, EggClockFace))
#define EGG_CLOCK_FACE_CLASS(obj)		(G_TYPE_CHECK_CLASS_CAST ((obj), EGG_CLOCK_FACE,  EggClockFaceClass))
#define EGG_IS_CLOCK_FACE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), EGG_TYPE_CLOCK_FACE))
#define EGG_IS_CLOCK_FACE_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), EGG_TYPE_CLOCK_FACE))
#define EGG_CLOCK_FACE_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS ((obj), EGG_TYPE_CLOCK_FACE, EggClockFaceClass))



typedef struct _EggClockFace		EggClockFace;
typedef struct _EggClockFaceClass	EggClockFaceClass;

struct _EggClockFace
{
	GtkDrawingArea parent;

	/* private */
};

struct _EggClockFaceClass
{
	GtkDrawingAreaClass parent_class;
};



GtkWidget* egg_clock_face_new (void);











