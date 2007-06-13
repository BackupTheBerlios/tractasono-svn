INCLUDES = $(PACKAGE_CFLAGS)

AM_CFLAGS = -Wall -g

AM_CPPFLAGS = -DDATADIR\="\"$(datadir)"\"

bin_PROGRAMS = tractasono

tractasono_SOURCES = drives.h interface.c player.c interface.h settings.c tractasono.h main.c player.h settings.h drives.c
	
tractasono_LDFLAGS = -export-dynamic

tractasono_LDADD = $(PACKAGE_LIBS)

