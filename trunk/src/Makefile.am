INCLUDES = $(GTK_CFLAGS) \
		   $(GNOMEVFS_CFLAGS) \
		   $(LIBGLADE_CFLAGS) \
		   $(GCONF_CFLAGS) \
		   $(GSTREAMER_CFLAGS) \
		   $(MUSICBRAINZ_CFLAGS) \
		   $(SQLITE_CFLAGS) \
		   $(LIBGPOD_CFLAGS) \
		   $(LIBIPODDEVICE_CFLAGS) \
		   $(TOTEMPLPARSE_CFLAGS) \
		   $(TAGLIB_CFLAGS)

AM_CFLAGS = -Wall -Werror -g

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
					 musicbrainz.c \
					 musicbrainz.h \
					 keyboard.c \
					 keyboard.h \
					 disc.c \
					 disc.h \
					 database.c \
					 database.h \
					 database_schema.c \
					 database_schema.h \
					 lcd.c \
					 lcd.h \
					 ipod.c \
					 ipod.h \
					 strukturen.c \
					 strukturen.h \
					 utils.c \
					 utils.h \
					 gtk-circle.c \
					 gtk-circle.h
	
tractasono_LDFLAGS = -export-dynamic

tractasono_LDADD = $(GTK_LIBS) \
				   $(GNOMEVFS_LIBS) \
				   $(LIBGLADE_LIBS) \
				   $(GCONF_LIBS) \
				   $(GSTREAMER_LIBS) \
				   $(MUSICBRAINZ_LIBS) \
				   $(SQLITE_LIBS) \
				   $(LIBGPOD_LIBS) \
				   $(LIBIPODDEVICE_LIBS) \
				   $(TOTEMPLPARSE_LIBS) \
				   $(TAGLIB_LIBS) \
				   -ltag_c

