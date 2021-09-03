#- aslstatus - async suckless status monitor -#

include config.mk


COMPONENTS  ?= $(wildcard components/*.c)
OBJ          = ${COMPONENTS:.c=.o}

X         ?= 1
XKB       ?= 1
AUDIO     ?= ALSA
A_ALSA_O  := components/volume/alsa.o
A_DEF_O   := components/volume/default.o
A_PULSE_O := components/volume/pulse.o


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
	echo ebat ${COMPONENTS}
endif


ifeq (${AUDIO},ALSA)
LDLIBS += ${LDALSA}
CPPFLAGS += -DUSE_ALSA
endif  # ALSA


ifeq (${AUDIO},PULSE)
LDLIBS += ${LDPULSE}
CPPFLAGS += -DUSE_PULSE
endif  # PULSE



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
	rm -f aslstatus aslstatus.o util.o ${OBJ} ${A_ALSA_O} ${A_PULSE_O} ${A_DEF_O}

.PHONY: uninstall
uninstall:
	rm -f "${DESTDIR}${PREFIX}/bin/aslstatus"
	rm -f "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"
