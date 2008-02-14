INCLUDES = $(GLIB_CFLAGS) \
		   $(GNOMEVFS_CFLAGS) \
		   -I/usr/include/taglib

AM_CFLAGS = -Wall

bin_PROGRAMS = spider

spider_SOURCES = spider.c

spider_LDADD = $(GLIB_LIBS) \
			   $(GNOMEVFS_LIBS) \
			   -ltag_c
