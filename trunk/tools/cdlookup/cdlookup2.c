#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "musicbrainz/mb_c.h"
#include "musicbrainz/browser.h"

int main (int argc, char *argv[])
{
	musicbrainz_t o;
	char url[1025];

	// Create the musicbrainz object, which will be needed for subsequent calls
	o = mb_New();

	// Set the proper server to use. Defaults to mm.musicbrainz.org:80
	if (getenv("MB_SERVER")) {
		mb_SetServer(o, getenv("MB_SERVER"), 80);
	}

	// Check to see if the debug env var has been set 
	if (getenv("MB_DEBUG")) {
		mb_SetDebug(o, atoi(getenv("MB_DEBUG")));
	}

	// Tell the server to only return 2 levels of data, unless the MB_DEPTH env var is set
	if (getenv("MB_DEPTH")) {
		mb_SetDepth(o, atoi(getenv("MB_DEPTH")));
	} else {
		mb_SetDepth(o, 2);
	}

	// Tell the client library to return data in ISO8859-1 and not UTF-8
	mb_UseUTF8(o, 0);

	// Now get the web submit url
	if (mb_GetWebSubmitURL(o, url, 1024)) {
		int ret;
		ret = LaunchBrowser(url, "firefox");
		if (ret == 0) {
			printf("Could not launch firefox browser!");
		}
	}

	// and clean up the musicbrainz object
	mb_Delete(o);

	return 0;
}
