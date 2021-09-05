#- aslstatus - async suckless status monitor -#

include config.mk


ifeq (${COMPONENTS},)
GLOB := $(shell case "$$(uname -s)" in \
		([Ff]ree[Bb][Ss][Dd]*) echo freebsd bsd;;\
		([Oo]pen[Bb][Ss][Dd]*) echo openbsd bsd;;\
		([Ll]inux*) echo linux;; esac)
COMPONENTS := $(wildcard components/*.c \
	      $(foreach _,${GLOB},components/*/${_}.c))
endif # COMPONENTS

OBJ          = ${COMPONENTS:.c=.o}

X         ?= 1
XKB       ?= 1
AUDIO     ?= ALSA


SMART_CONFIG ?= 1


ifeq (${X},1)
LDLIBS   += ${LDXCB}
CPPFLAGS += -DUSE_X=1

ifeq (${XKB},1)
LDLIBS   += ${LDXCB_XKB}
CPPFLAGS += -DUSE_XKB=1
else
CPPFLAGS += -DUSE_XKB=0
endif  # XKB

else
CPPFLAGS += -DUSE_X=0
CPPFLAGS += -DUSE_XKB=0
endif  # X

.PHONY: all
ifeq (${SMART_CONFIG},1)
all: smart-conf
include smart-config.mk
else
all: aslstatus

A_ALSA_C  := components/volume/alsa.c
A_DEF_C   := components/volume/default.c
A_PULSE_C := components/volume/pulse.c
endif


ifeq (${AUDIO},ALSA)
LDLIBS     += ${LDALSA}
CPPFLAGS   += -DUSE_ALSA
COMPONENTS += ${A_ALSA_C}
endif  # ALSA


ifeq (${AUDIO},PULSE)
LDLIBS     += ${LDPULSE}
CPPFLAGS   += -DUSE_PULSE
COMPONENTS += ${A_PULSE_C}
endif  # PULSE

ifneq (${AUDIO},ALSA)
ifneq (${AUDIO},PULSE)
COMPONENTS += ${A_DEF_C}
endif
endif



${OBJ}: util.h components_config.h
aslstatus.o: aslstatus.c aslstatus.h config.h util.h os.h

${OBJ} util.o: %.o: %.c
	$(CC) -o $@ -c $< ${CFLAGS} ${CPPFLAGS}

aslstatus: aslstatus.o util.o ${OBJ}
	$(CC) -o $@  $^ ${LDFLAGS} ${LDLIBS} ${CFLAGS}

aslstatus.1: aslstatus.1.md
	pandoc --standalone --from=markdown $< --to=man -o $@

.PHONY: man
man: aslstatus.1

.PHONY: install
install: all
	mkdir -p "${DESTDIR}${PREFIX}/bin"
	cp -f aslstatus "${DESTDIR}${PREFIX}/bin"
	chmod 755 "${DESTDIR}${PREFIX}/bin/aslstatus"
	mkdir -p "${DESTDIR}${MANPREFIX}/man1"
	cp -f aslstatus.1 "${DESTDIR}${MANPREFIX}/man1"
	chmod 644 "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"

.PHONY: clean
ifeq (${SMART_CONFIG},1)
clean: smart-config-clean
endif
clean:
	rm -f aslstatus aslstatus.o util.o \
		$(wildcard components/*.o components/*/*.o)

.PHONY: uninstall
uninstall:
	rm -f "${DESTDIR}${PREFIX}/bin/aslstatus"
	rm -f "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"
