#- aslstatus version -#
VERSION = 0.1

#- customize below to fit your system -#

#- paths -#
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

#- flags -#
CPPFLAGS = -I${X11INC} -D_DEFAULT_SOURCE
WNO      = -Wno-unused-parameter
# LTO      = -flto
# OPT      = -march=native -fPIE -funroll-loops
CFLAGS   = -std=c99 -O3 -fPIC -pedantic -Wall -Wextra ${WNO} ${OPT} ${LTO}

LDFLAGS  = -L${X11LIB} -s ${LTO}
LDLIBS   = -lX11 -lpthread #-Wl,-O3 -Wl,--as-needed

LDALSA   = -lasound
LDPULSE  = -lpulse



#- compiler and linker -#
# CC = clang
CC = gcc

#- strip -#
STRIPFLAGS = -s
# STRIPFLAGS = --strip-unneeded -R .comment  # less agresive strip

# NOSTRIP = 1  # uncomment to leave binary unstriped
# FSTRIP  = 1  # uncomment to force strip even though compiler unknown


ifeq ($(CC), clang)
STRIP = llvm-strip
STRIPFLAGS += --strip-all-gnu

else ifeq ($(CC), gcc)
STRIP = strip
STRIPFLAGS += -R .GCC.command.line -R .note.gnu.gold-version

else ifeq (${FSTRIP}, 1)
STRIP = strip

else
NOSTRIP = 1

endif
