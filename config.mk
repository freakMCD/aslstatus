#- aslstatus version -#
VERSION := 0.1

CC ?= cc

#- paths -#
PREFIX    := /usr
MANPREFIX := ${PREFIX}/share/man

PULSELIB  := /usr/lib/pulseaudio

#- flags -#
CPPFLAGS += -D_DEFAULT_SOURCE
CFLAGS   += -std=c99 -pedantic -Wall -Wextra

#- linker -#
LDFLAGS  += -L${X11LIB} -s
LDLIBS   := -lpthread -lxcb -lxcb-xkb
LDALSA   := -lasound
LDPULSE  := -lpulse -L${PULSELIB}
