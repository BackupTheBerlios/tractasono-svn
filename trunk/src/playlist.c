/*
 *      playlist.c
 *      
 *      Copyright 2008 Patrik Obrist <patrik@gmx.net>
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

#include <glib.h>
#include "playlist.h"


// Neue PlayList erstellen
PlayList* playlist_new (void)
{
	PlayList *playlist;
	
	playlist = g_new0 (PlayList, 1);
	playlist->list = g_list_alloc ();
		
	return playlist;
}


// PlayList Speicher wieder freigeben
void playlist_free (PlayList *playlist)
{
	g_return_if_fail (playlist != NULL);
	
	// einzelne Pfade zuerst freigeben
	GList *l;
	for (l = playlist->list; l != NULL; l = l->next) {
		g_free (l->data);
	}
	g_list_free (playlist->list);
	
	g_free (playlist);
}


// Fügt eine URI (Pfad) zur Liste hinzu
void playlist_add_uri (PlayList *playlist, const gchar *uri)
{
	g_return_if_fail (playlist != NULL);
	g_return_if_fail (uri != NULL);
	
	playlist->list = g_list_append (playlist->list, g_strdup (uri));
	playlist->list->data = g_strdup (uri);
	
	g_debug ("Aktuelle URI: %s", playlist_get_uri (playlist));
}


// Gibt die aktuelle URI zurück
gchar* playlist_get_uri (PlayList *playlist)
{
	g_return_val_if_fail (playlist != NULL, NULL);
	
	if (!playlist->list->data) {
		return NULL;
	}
	
	return (gchar*) playlist->list->data;
}


// Springt zum nächsten Eintrag in der Liste
// Beim Erreichen des Endes gibts NULL zurück
gboolean playlist_next (PlayList *playlist)
{
	g_return_val_if_fail (playlist != NULL, FALSE);
	
	GList *temp_list;
	
	temp_list = g_list_next (playlist->list);
	if (!temp_list) {
		return FALSE;
	}
	playlist->list = temp_list;
	
	return TRUE;
}


// Springt zum vorherigen Eintrag in der Liste
// Beim Erreichen des Starts gibts NULL zurück
gboolean playlist_prev (PlayList *playlist)
{
	g_return_val_if_fail (playlist != NULL, FALSE);
	
	GList *temp_list;
	
	temp_list = g_list_previous (playlist->list);
	if (!temp_list) {
		return FALSE;
	}
	playlist->list = temp_list;
	
	return TRUE;
}


// Rückt zur nth Position in der Liste vor
// Falls die Position nicht existiert gibts NULL zurück
gboolean playlist_nth (PlayList *playlist, gint pos)
{
	g_return_val_if_fail (playlist != NULL, FALSE);
	
	GList *temp_list;
	
	temp_list = g_list_nth (playlist->list, pos);
	if (!temp_list) {
		return FALSE;
	}
	playlist->list = temp_list;
	
	return TRUE;
}
