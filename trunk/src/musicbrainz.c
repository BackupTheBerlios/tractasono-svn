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
#include <musicbrainz/mb_c.h>
#include <musicbrainz/browser.h>

#include "musicbrainz.h"



void musicbrainz_init (void)
{
	g_message ("\tMusicbrainz init");
}



gboolean musicbrainz_lookup_disc (gchar *device, gchar *discid)
{
	musicbrainz_t o;
	char error[256], data[256], temp[256], *args[2];
	int ret, numTracks, albumNum;

	// Create the musicbrainz object, which will be needed for subsequent calls
	o = mb_New();

	// enable debug
	mb_SetDebug(o, TRUE);

	// Tell the client library to return data in ISO8859-1 and not UTF-8
	mb_UseUTF8(o, 0);

	// Tell the server to only return 4 levels of data, unless the MB_DEPTH env var is set
	mb_SetDepth(o, 4);

	// Set up the args for the find album query
	args[0] = discid;
	args[1] = NULL;

	if (strlen(discid) != MB_CDINDEX_ID_LEN) {
		// Execute the MB_GetAlbumById query
		ret = mb_QueryWithArgs(o, MBQ_GetAlbumById, args);
	} else {
		// Execute the MBQ_GetCDInfoFromCDIndexId
		ret = mb_QueryWithArgs(o, MBQ_GetCDInfoFromCDIndexId, args);
	}

	if (!ret) {
		mb_GetQueryError(o, error, 256);
		printf("Query failed: %s\n", error);
		mb_Delete(o);
		return FALSE;
	}

	return TRUE;

	// Select the first album
	if (!mb_Select1(o, MBS_SelectAlbum, albumNum)) {
		return FALSE;
	}
	
	// Pull back the album id to see if we got the album
	if (!mb_GetResultData(o, MBE_AlbumGetAlbumId, data, 256)) {
		printf("Album not found.\n");
		return FALSE;
	}

	printf("Match #: %d\n-------------------------------------------------\n", albumNum);
	mb_GetIDFromURL(o, data, temp, 256);
	printf("    AlbumId: %s\n", temp);

	// Extract the album name
	if (mb_GetResultData(o, MBE_AlbumGetAlbumName, data, 256)) {
		printf("       Name: %s\n", data);
	}

	// Extract the number of tracks
	numTracks = mb_GetResultInt(o, MBE_AlbumGetNumTracks);
	if (numTracks > 0 && numTracks < 100) {
		printf("  NumTracks: %d\n", numTracks);
	}

	// Extract the artist name from the album
	if (mb_GetResultData1(o, MBE_AlbumGetArtistName, data, 256, 1)) {
		printf("AlbumArtist: %s\n", data);
	}

	// Extract the artist id from the album
	if (mb_GetResultData1(o, MBE_AlbumGetArtistId, data, 256, 1)) {
		mb_GetIDFromURL(o, data, temp, 256);
		printf("   ArtistId: %s\n", temp);
	}

	// and clean up the musicbrainz object
	mb_Delete(o);

	return TRUE;
}


gchar* musicbrainz_get_disctitle (void)
{	
	/*if (strcmp (m_disc.disctitle, "") == 0) {
		return m_disc.discid;
	} else {
		return m_disc.disctitle;
	}*/
	
	return "";
}

