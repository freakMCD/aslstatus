# 1: shell scrtipt
# 2: compiled executable
# 3: components directory

smart_config = $(shell sh -- $1 $2 $3)


_SC     := smart-config
_GEN    := ${_SC}/gen_comp.sed
_EXE    := ${_SC}/smart-config
_SH     := ${_SC}/smart-config.sh
_COMP_C := ${_SC}/components.c
_COMP_O := ${_COMP_C:.c=.o}

SMART_CONFIG_EXE          := ${_EXE}
SMART_CONFIG_COMPONENTS_O := ${_COMP_O}

SMART_CONFIG_OFILES := ${_EXE}.o ${_COMP_O}
SMART_CONFIG_DEPS   := ${_EXE} ${_SH}

SMART_CONFIG_FILTER := man install clean uninstall

${_COMP_O}: lib/util.h
${_EXE}.o: lib/util.h aslstatus.h config.h

.PHONY: smart-conf
smart-conf: ${SMART_CONFIG_DEPS}
	@echo 'USING SMART CONFIG'
	$(eval export SMART_CONFIG=0)
	$(eval export COMPONENTS=$(addprefix components/,\
		$(call smart_config,./${_SH},./${_EXE},./components)))
	@+$(MAKE) $(filter-out ${SMART_CONFIG_FILTER},$(MAKECMDGOALS))

${_EXE}: ${SMART_CONFIG_OFILES}
	$(CC) -o $@ $^ ${LDFLAGS} ${CFLAGS} -Wno-lto-type-mismatch

.PHONY: smart-config-clean
smart-config-clean:
	rm -f -- ${SMART_CONFIG_EXE} ${SMART_CONFIG_OFILES}

${_COMP_C}: aslstatus.h ${_GEN}
	sed -nf ./${_GEN} < $< > $@
