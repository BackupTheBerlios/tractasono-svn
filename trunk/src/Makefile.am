INCLUDES = $(LIBS_CFLAGS)

AM_CFLAGS = -Wall -Werror -g
AM_CPPFLAGS = -DINSTALL_PREFIX=\"$(prefix)\" \
			  -DDATADIR\="\"$(datadir)"\" \
			  -DICON_DIR=\"$(datadir)/pixmaps\"

bin_PROGRAMS = tractasono

tractasono_LDADD = $(LIBS_LIBS)
tractasono_LDFLAGS = -export-dynamic
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
					 lcd.c \
					 lcd.h \
					 playlist.c \
					 playlist.h \
					 strukturen.c \
					 strukturen.h \
					 utils.c \
					 utils.h
