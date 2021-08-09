#- aslstatus version -#
VERSION := 0.1

CC ?= cc

#- paths -#
PREFIX    := /usr
MANPREFIX := ${PREFIX}/share/man

#- flags -#
CPPFLAGS += -D_DEFAULT_SOURCE '-DVERSION="${VERSION}"'
CFLAGS   += -std=c99 -pedantic -Wall -Wextra

#- linker -#
pkgconf   = $(shell pkg-config --libs $1)

LDLIBS  := -lpthread -pthread
LDALSA   = $(call pkgconf,alsa)  # -lasound
LDPULSE  = $(call pkgconf,--static libpulse)  # -lpulse -L/usr/lib/pulseaudio

LDXCB     = $(call pkgconf,xcb)  # -lxcb
LDXCB_XKB = $(call pkgconf,xcb-xkb)  # -lxcb-xkb
