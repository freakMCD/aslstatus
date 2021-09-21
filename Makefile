#- aslstatus - async suckless status monitor -#

include config.mk


ifeq (${COMPONENTS},)
COMPONENTS := $(wildcard components/*.c)
COMPONENTS += $(wildcard lib/*.c)

A_ALSA_C  := components/volume/alsa.c
A_PULSE_C := components/volume/pulse.c
A_DEF_C   := components/volume/default.c
endif # COMPONENTS

OBJ = ${COMPONENTS:.c=.o}

X     ?= 1
XKB   ?= 1
AUDIO ?= ALSA

# components that needs running X server
NEED_X_SERVER_COM := keymap

NEED_X_SERVER_DEF := -DNEED_X_SERVER=1

NEED_X_SERVER = $(if \
			$(strip \
			$(foreach _,\
				${NEED_X_SERVER_COM},\
				$(filter %/${_}.c,${COMPONENTS})\
			)),\
		${NEED_X_SERVER_DEF},)

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

ifneq (,$(filter %tcc,${CC})) # if compiler is tcc
# libraries and flags first, then files to link
LINK_FLAGS = ${LDFLAGS} ${LDLIBS} ${CFLAGS} ${1}
else
# vice versa
LINK_FLAGS = ${1} ${LDFLAGS} ${LDLIBS} ${CFLAGS}
endif

include deps.mk

aslstatus.o: CPPFLAGS += $(NEED_X_SERVER)

%.o: %.c
	$(CC) -o $@ -c $< ${CFLAGS} ${CPPFLAGS}

aslstatus: aslstatus.o ${OBJ}
	$(CC) -o $@ $(call LINK_FLAGS,$^)

man/aslstatus.1: man/aslstatus.1.md
	pandoc --standalone --from=markdown $< --to=man -o $@

.PHONY: man
man: man/aslstatus.1

.PHONY: install
install: all
	mkdir -p "${DESTDIR}${PREFIX}/bin"
	cp -f aslstatus "${DESTDIR}${PREFIX}/bin"
	chmod 755 "${DESTDIR}${PREFIX}/bin/aslstatus"
	mkdir -p "${DESTDIR}${MANPREFIX}/man1"
	cp -f man/aslstatus.1 "${DESTDIR}${MANPREFIX}/man1"
	chmod 644 "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"

.PHONY: clean
ifeq (${SMART_CONFIG},1)
clean: smart-config-clean
endif
clean:
	rm -f aslstatus aslstatus.o \
		$(wildcard lib/*.o components/*.o components/*/*.o)

.PHONY: uninstall
uninstall:
	rm -f "${DESTDIR}${PREFIX}/bin/aslstatus"
	rm -f "${DESTDIR}${MANPREFIX}/man1/aslstatus.1"
