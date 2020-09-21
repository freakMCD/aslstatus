#- aslstatus - async suckless status monitor -#
.POSIX:

include config.mk

REQ = util
COM = $(wildcard components/*.c)
OBJ = ${COM:.c=.o}

ifeq (${AUDIO}, ALSA)
	audio   = -DUSE_${AUDIO}
	LDLIBS += ${LDALSA}
else ifeq (${AUDIO}, PULSE)
	audio   = -DUSE_${AUDIO}
	LDLIBS += ${LDPULSE}
endif


all: aslstatus


${OBJ}: config.mk ${REQ:=.h}
aslstatus.o: aslstatus.c aslstatus.h config.h config.mk ${REQ:=.h}


.c.o:
	$(CC) -o $@ -c ${CPPFLAGS} ${CFLAGS} $(audio) $<


aslstatus: aslstatus.o ${OBJ} ${REQ:=.o}
	$(CC) -o $@ ${LDFLAGS} ${OBJ} ${REQ:=.o} $< ${LDLIBS}
ifndef NOSTRIP
	$(STRIP) ${STRIPFLAGS} $@
endif


clean:
	rm -f aslstatus aslstatus.o ${OBJ} ${REQ:=.o}


install: all
	mkdir -p "${DESTDIR}${PREFIX}/bin"
	cp -f aslstatus "${DESTDIR}${PREFIX}/bin"
	chmod 755 "${DESTDIR}${PREFIX}/bin/aslstatus"
	mkdir -p "${DESTDIR}${MANPREFIX}/man1"
	cp -f aslstatus.1 "${DESTDIR}${MANPREFIX}/man1"
	chmod 644 "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"


uninstall:
	rm -f "${DESTDIR}${PREFIX}/bin/aslstatus"
	rm -f "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"
