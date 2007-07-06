INCLUDES = $(PACKAGE_CFLAGS)

AM_CFLAGS = -Wall -g

AM_CPPFLAGS = -DDATADIR\="\"$(datadir)"\"

bin_PROGRAMS = tractasono

tractasono_SOURCES =	main.c \
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
			database.h
	
tractasono_LDFLAGS = -export-dynamic

tractasono_LDADD = $(PACKAGE_LIBS)

