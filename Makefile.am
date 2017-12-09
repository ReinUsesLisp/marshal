ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libmarshal.la
libmarshal_la_CFLAGS = -ansi -DMARSHAL_BUILDING
libmarshal_la_SOURCES = \
	src/format.h \
	src/clone.c \
	src/equal.c \
	src/decode.c \
	src/encode.c \
	src/free.c \
	src/print.c \
	src/encoding.c \
	src/access.c \
	src/make.c
pkginclude_HEADERS = src/marshal.h
