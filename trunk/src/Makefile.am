INCLUDES = $(GTK_CFLAGS) \
		   $(GNOMEVFS_CFLAGS) \
		   $(LIBGLADE_CFLAGS) \
		   $(GCONF_CFLAGS) \
		   $(GSTREAMER_CFLAGS) \
		   $(LIBGDA_CFLAGS) \
		   $(LIBGNOMEDB_CFLAGS) \
		   $(LIBMUSICBRAINZ_CFLAGS)

AM_CFLAGS = -Wall -g

AM_CPPFLAGS = -DINSTALL_PREFIX=\"$(prefix)\" \
			  -DDATADIR\="\"$(datadir)"\" \
			  -DGNOME_ICONDIR=\""$(datadir)/pixmaps"\"

bin_PROGRAMS = tractasono

tractasono_SOURCES = main.c \
					 drives.c \
					 drives.h \
					 interface.c \
					 interface.h \
					 player.c \
					 player.h \
					 settings.c \
					 settings.h \
					 radio.c \
					 radio.h \
					 fullscreen.c \
					 fullscreen.h \
					 music.c \
					 music.h \
					 keyboard.c \
					 keyboard.h \
					 disc.c \
					 disc.h \
					 database.c \
					 database.h \
					 musicbrainz.c \
					 musicbrainz.h \
					 lcd.c \
					 lcd.h
	
tractasono_LDFLAGS = -export-dynamic

tractasono_LDADD = $(GTK_LIBS) \
				   $(GNOMEVFS_LIBS) \
				   $(LIBGLADE_LIBS) \
				   $(GCONF_LIBS) \
				   $(GSTREAMER_LIBS) \
				   $(LIBGDA_LIBS) \
				   $(LIBGNOMEDB_LIBS) \
				   $(LIBMUSICBRAINZ_LIBS)
