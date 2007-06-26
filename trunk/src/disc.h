/*
 *      disc.h
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
#include <glade/glade.h>


/**
 * Toggle, Title and Artist Renderers
 */
GtkCellRenderer *toggle_renderer, *title_renderer, *artist_renderer;

/**
 * The columns in the list view
 */
typedef enum {
  COLUMN_STATE,
  COLUMN_EXTRACT,
  COLUMN_NUMBER,
  COLUMN_TITLE,
  COLUMN_ARTIST,
  COLUMN_DURATION,
  COLUMN_DETAILS,
  COLUMN_TOTAL
} ViewColumn;

typedef enum {
  STATE_IDLE,
  STATE_PLAYING,
  STATE_EXTRACTING
} TrackState;

typedef enum {
  DEBUG_CD = 1 << 0,
  DEBUG_METADATA = 1 << 1,
  DEBUG_EXTRACTING = 1 << 2,
  DEBUG_PLAYING = 1 << 3,
} SjDebugDomain;



typedef struct _AlbumDetails AlbumDetails;
typedef struct _TrackDetails TrackDetails;


struct _TrackDetails {
  AlbumDetails *album;
  int number; /* track number */
  char *title;
  char *artist;
  char* artist_sortname; /* Can be NULL, so fall back onto artist */
  int duration; /* seconds */
  char* track_id;
  char* artist_id;
  GtkTreeIter iter; /* Temporary iterator for internal use */
};

struct _AlbumDetails {
  char* title;
  char* artist;
  char* artist_sortname;
  char *genre;
  int   number; /* number of tracks in the album */
  GList* tracks;
  GDate *release_date; /* MusicBrainz support multiple releases per album */
  char* album_id;
  char* artist_id;
};






GtkListStore *track_store;
GtkWidget *treeview;



void disc_init(void);

void on_extract_toggled (GtkCellRendererToggle *cellrenderertoggle,
                                gchar *path,
                                gpointer user_data);


 
void number_cell_icon_data_cb (GtkTreeViewColumn *tree_column,
				      GtkCellRenderer *cell,
				      GtkTreeModel *tree_model,
				      GtkTreeIter *iter,
				      gpointer data);
				      
				   
void on_cell_edited (GtkCellRendererText *renderer,
                 gchar *path, gchar *string,
                 gpointer column_data);
                 
                 
void duration_cell_data_cb (GtkTreeViewColumn *tree_column,
                                GtkCellRenderer *cell,
                                GtkTreeModel *tree_model,
                                GtkTreeIter *iter,
                                gpointer data);


