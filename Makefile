#- aslstatus - async suckless status monitor -#

include config.mk


COMPONENTS  := $(wildcard components/*.c)
OBJ          = ${COMPONENTS:.c=.o}

NEED_VOLUME := aslstatus.o

AUDIO     ?= ALSA
A_ALSA_O  := components/volume/alsa.o
A_DEF_O   := components/volume/default.o
A_PULSE_O := components/volume/pulse.o


.PHONY: all
all: aslstatus


ifeq (${AUDIO},ALSA)
LDFLAGS += ${LDALSA}
COMPONENTS += ${A_ALSA_O:.o=.c}

${NEED_VOLUME}: CPPFLAGS += -DUSE_ALSA
endif  # ALSA


ifeq (${AUDIO},PULSE)
LDFLAGS += ${LDPULSE}
COMPONENTS += ${A_PULSE_O:.o=.c}

${NEED_VOLUME}: CPPFLAGS += -DUSE_PULSE
endif  # PULSE


ifneq (${AUDIO},ALSA)
ifneq (${AUDIO},PULSE)
COMPONENTS += ${A_DEF_O:.o=.c}
endif  # PULSE
endif  # ALSA

${NEED_VOLUME}: components/volume/volume.h


${OBJ}: util.h
aslstatus.o: aslstatus.c aslstatus.h config.h util.h os.h

${OBJ} util.o: %.o: %.c
	$(CC) -o $@ -c $< ${CFLAGS} ${CPPFLAGS}

aslstatus: aslstatus.o util.o ${OBJ}
	$(CC) -o $@ ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} ${LDLIBS} $^

.PHONY: install
install: all
	mkdir -p "${DESTDIR}${PREFIX}/bin"
	cp -f aslstatus "${DESTDIR}${PREFIX}/bin"
	chmod 755 "${DESTDIR}${PREFIX}/bin/aslstatus"
	mkdir -p "${DESTDIR}${MANPREFIX}/man1"
	cp -f aslstatus.1 "${DESTDIR}${MANPREFIX}/man1"
	chmod 644 "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"

.PHONY: clean
clean:
	rm -f aslstatus aslstatus.o util.o ${OBJ} ${A_ALSA_O} ${A_PULSE_O} ${A_DEF_O}

.PHONY: uninstall
uninstall:
	rm -f "${DESTDIR}${PREFIX}/bin/aslstatus"
	rm -f "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"
