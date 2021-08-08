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
pkgconf   = $(shell pkg-config --static --libs --cflags-only-I $1)

LDLIBS   := $(call pkgconf,xcb xcb-xkb) -lpthread -pthread
LDALSA   := $(call pkgconf,alsa)
LDPULSE  := $(call pkgconf,libpulse)
