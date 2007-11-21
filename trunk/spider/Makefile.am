INCLUDES = $(GTK_CFLAGS) \
		   $(TAGLIB_CFLAGS)

AM_CFLAGS = -Wall -g -ltag_c

AM_CPPFLAGS = -DINSTALL_PREFIX=\"$(prefix)\"

bin_PROGRAMS = spider

spider_SOURCES = main.c

spider_LDFLAGS = -export-dynamic

spider_LDADD = $(GTK_LIBS) \
			   $(TAGLIB_LIBS)
