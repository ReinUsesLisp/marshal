CFLAGS="-O2 -Wall -Werror -ansi"
LDFLAGS="-lmarshal"
gcc $CFLAGS $LDFLAGS -o printer printer.c
gcc $CFLAGS $LDFLAGS -o re-encode re-encode.c
