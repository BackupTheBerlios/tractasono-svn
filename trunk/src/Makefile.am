INCLUDES = $(GTK_CFLAGS) \
		   $(GIO_CFLAGS) \
		   $(GLADE_CFLAGS) \
		   $(GSTREAMER_CFLAGS) \
		   $(MUSICBRAINZ_CFLAGS) \
		   $(SQLITE_CFLAGS) \
		   $(TOTEMPLPARSE_CFLAGS) \
		   $(TAGLIB_CFLAGS)

AM_CFLAGS = -Wall -Werror -g

AM_CPPFLAGS = -DINSTALL_PREFIX=\"$(prefix)\" \
			  -DDATADIR\="\"$(datadir)"\" \
			  -DICON_DIR=\"$(datadir)/pixmaps\"

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
				   $(GIO_LIBS) \
				   $(GLADE_LIBS) \
				   $(GSTREAMER_LIBS) \
				   $(MUSICBRAINZ_LIBS) \
				   $(SQLITE_LIBS) \
				   $(TOTEMPLPARSE_LIBS) \
				   $(TAGLIB_LIBS) \
				   -ltag_c

