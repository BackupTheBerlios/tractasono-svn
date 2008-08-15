/*
 *      musicbrainz.c
 *      
 *      Copyright 2007 Patrik Obrist <patrik@valhalla>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "musicbrainz.h"
#include "strukturen.h"


// Variablen
musicbrainz_t *mb;


// Prototypen
static int get_duration_from_sectors (int sectors);
static void convert_encoding (char **str);
static void artist_and_title_from_title (TrackDetails *track, gpointer data);
AlbumDetails* get_album_offline (void);


void musicbrainz_init (void)
{
	g_message ("Musicbrainz init");
	
	// Musicbrainz init
	mb = mb_New ();
	
}



AlbumDetails* musicbrainz_lookup_offline (void)
{
	AlbumDetails *album;
	TrackDetails *track;
	int num_tracks, i;
	
	if (mb == NULL) {
		return NULL;
	}
	
	/* Get the Table of Contents */
	if (!mb_Query (mb, MBQ_GetCDTOC)) {
		return NULL;
	}

	if (!mb_Query (mb, MBQ_GetCDInfo)) {
		return NULL;
	}
	
	num_tracks = mb_GetResultInt (mb, MBE_TOCGetLastTrack);

	album = g_new0 (AlbumDetails, 1);
	album->artist = g_strdup ("Unknown Artist");
	album->title = g_strdup ("Unknown Title");
	album->genre = g_strdup ("Unknown Genre");
	
	for (i = 1; i <= num_tracks; i++) {
		track = g_new0 (TrackDetails, 1);
		track->album = album;
		track->number = i;
		track->title = g_strdup_printf ("Track %d", i);
		track->artist = g_strdup (album->artist);
		track->duration = get_duration_from_sectors (mb_GetResultInt1 (mb, MBE_TOCGetTrackNumSectors, i+1));
		album->tracks = g_list_append (album->tracks, track);
		album->number++;
	}
	
	return album;
}




AlbumDetails* musicbrainz_lookup_cd (void)
{
	AlbumDetails *album; 
	GList *tl;
	char data[256];
	int num_albums, j;
	
	/* Get the Table of Contents */
	if (!mb_Query (mb, MBQ_GetCDTOC)) {
		return NULL;
	}

	if (!mb_Query (mb, MBQ_GetCDInfo)) {
		return NULL;
	}
	
	num_albums = mb_GetResultInt(mb, MBE_GetNumAlbums);
	if (num_albums < 1) {
		album = musicbrainz_lookup_offline ();
		return album;
	}
		
	int num_tracks;
	gboolean from_freedb = FALSE;

	mb_Select1(mb, MBS_SelectAlbum, 1);
	album = g_new0 (AlbumDetails, 1);
	
	// default values
	album->genre = g_strdup ("Unbekannt");
	album->compilation = FALSE;
	album->release_date = NULL;

	if (mb_GetResultData(mb, MBE_AlbumGetAlbumId, data, sizeof (data))) {
		from_freedb = strstr(data, "freedb:") == data;
		mb_GetIDFromURL (mb, data, data, sizeof (data));
		album->album_id = g_strdup (data);
	}

	if (mb_GetResultData (mb, MBE_AlbumGetAlbumArtistId, data, sizeof (data))) {
		//g_message ("1");
		mb_GetIDFromURL (mb, data, data, sizeof (data));
		album->artist_id = g_strdup (data);

		if (mb_GetResultData (mb, MBE_AlbumGetAlbumArtistName, data, sizeof (data))) {
			//g_message ("2");
			album->artist = g_strdup (data);
		} else {
			//g_message ("3");
			if (g_ascii_strcasecmp (MBI_VARIOUS_ARTIST_ID, album->artist_id) == 0) {
				//g_message ("4");
				album->artist = g_strdup ("Various Artist");
				album->compilation = TRUE;
			} else {
				//g_message ("5");
				album->artist = g_strdup ("Unknown Artist");
			}
		}
		//g_message ("id: %s", album->artist_id);
		if (g_ascii_strcasecmp (MBI_VARIOUS_ARTIST_ID, album->artist_id) == 0) {
			//g_message ("6");
			album->compilation = TRUE;
		}
		if (g_ascii_strcasecmp (album->artist, "Various Artist") == 0) {
			//g_message ("7");
			album->compilation = TRUE;
		}
		if (g_ascii_strcasecmp (album->artist, "Various Artists") == 0) {
			//g_message ("8");
			album->compilation = TRUE;
		}
	}

	// Disc title
	if (mb_GetResultData(mb, MBE_AlbumGetAlbumName, data, sizeof (data))) {
		album->title = g_strdup (data);
	} else {
		album->title = g_strdup ("Unknown Title");
	}

	{
		int num_releases;
		num_releases = mb_GetResultInt (mb, MBE_AlbumGetNumReleaseDates);
		if (num_releases > 0) {
			mb_Select1(mb, MBS_SelectReleaseDate, 1);
			if (mb_GetResultData(mb, MBE_ReleaseGetDate, data, sizeof (data))) {
				int matched, year=1, month=1, day=1;
				matched = sscanf(data, "%u-%u-%u", &year, &month, &day);
				if (matched >= 1) {
					album->release_date = g_date_new_dmy ((day == 0) ? 1 : day, (month == 0) ? 1 : month, year);
				}
			}
			mb_Select(mb, MBS_Back);
		}
	}

	num_tracks = mb_GetResultInt(mb, MBE_AlbumGetNumTracks);
	if (num_tracks < 1) {
		g_free (album->artist);
		g_free (album->title);
		g_free (album);
		g_warning ("Incomplete metadata for this CD");
		return NULL;
	}

	for (j = 1; j <= num_tracks; j++) {
		TrackDetails *track;
		track = g_new0 (TrackDetails, 1);

		track->album = album;
		track->number = j; /* replace with number lookup? */

		if (mb_GetResultData1(mb, MBE_AlbumGetTrackId, data, sizeof (data), j)) {
			mb_GetIDFromURL (mb, data, data, sizeof (data));
			track->track_id = g_strdup (data);
		}

		if (mb_GetResultData1(mb, MBE_AlbumGetArtistId, data, sizeof (data), j)) {
			mb_GetIDFromURL (mb, data, data, sizeof (data));
			track->artist_id = g_strdup (data);
		}

		if (mb_GetResultData1(mb, MBE_AlbumGetTrackName, data, sizeof (data), j)) {
			if (track->artist_id != NULL) {
				artist_and_title_from_title (track, data);
			} else {
				track->title = g_strdup (data);
			}
		} else {
			track->title = g_strdup ("[Untitled]");
		}

		if (track->artist == NULL && mb_GetResultData1(mb, MBE_AlbumGetArtistName, data, sizeof (data), j)) {
			track->artist = g_strdup (data);
		}

		if (mb_GetResultData1(mb, MBE_AlbumGetTrackDuration, data, sizeof (data), j)) {
			track->duration = atoi (data) / 1000;
		}

		if (from_freedb) {
			convert_encoding (&track->title);
			convert_encoding (&track->artist);
		}
  
		album->tracks = g_list_append (album->tracks, track);
		album->number++;
	}

	if (from_freedb) {
		convert_encoding (&album->title);
		convert_encoding (&album->artist);
	}

	mb_Select (mb, MBS_Rewind);
	

	/* For each album, we need to insert the duration data if necessary
	* We need to query this here because otherwise we would flush the
	* data queried from the server */
	/* TODO: scan for 0 duration before doing the query to avoid another lookup if
	we don't need to do it */
	mb_Query (mb, MBQ_GetCDTOC);
	j = 1;
	for (tl = album->tracks; tl; tl = tl->next) {
		TrackDetails *track = tl->data;
		int sectors;

		if (track->duration == 0) {
			sectors = mb_GetResultInt1 (mb, MBE_TOCGetTrackNumSectors, j+1);
			track->duration = get_duration_from_sectors (sectors);
		}
		j++;
	}
	
	return album;
}



static int get_duration_from_sectors (int sectors)
{
	#define BYTES_PER_SECTOR 2352
	#define BYTES_PER_SECOND (44100 / 8) / 16 / 2
	
	return (sectors * BYTES_PER_SECTOR / BYTES_PER_SECOND);
}


/*
 * Magic character set encoding to try and repair brain-dead FreeDB encoding,
 * converting it to the current locale's encoding (which is likely to be the
 * intended encoding).
 */
static void convert_encoding (char **str)
{
	char *iso8859;
	char *converted;

	if (str == NULL || *str == NULL) {
		return;
	}

	iso8859 = g_convert (*str, -1, "ISO-8859-1", "UTF-8", NULL, NULL, NULL);

	if (iso8859) {
		converted = g_locale_to_utf8 (iso8859, -1, NULL, NULL, NULL);

		if (converted) {
	  		g_free (*str);
	  		*str = converted;
		}
	}
	
	g_free (iso8859);
}


/* Data imported from FreeDB is horrendeous for compilations,
 * Try to split the 'Various' artist */
static void artist_and_title_from_title (TrackDetails *track, gpointer data)
{
	char *slash, **split;

	if (g_ascii_strncasecmp (track->album->album_id, "freedb:", 7) != 0 && track->album->artist_id[0] != '\0' && track->artist_id[0] != '\0') {
		track->title = g_strdup (data);
		return;
	}

	slash = strstr (data, " / ");
	if (slash == NULL) {
		track->title = g_strdup (data);
		return;
	}
	split = g_strsplit (data, " / ", 2);
	track->artist = g_strdup (split[0]);
	track->title = g_strdup (split[1]);
	g_strfreev (split);
}


