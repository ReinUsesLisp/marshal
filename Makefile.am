ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libmarshal.la
libmarshal_la_CFLAGS = -ansi -DMARSHAL_BUILDING
libmarshal_la_SOURCES = \
	src/format.h \
	src/clone.c \
	src/decode.c \
	src/free.c \
	src/print.c
pkginclude_HEADERS = src/marshal.h
