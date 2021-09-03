# 1: shell scrtipt
# 2: compiled executable
# 3: components directory

smart_config = $(shell sh -- $1 $2 $3)


_EXE    := smart-config/smart-config
_SH     := smart-config/smart-config.sh
_COMP_O := smart-config/components.o

SMART_CONFIG_EXE          := ${_EXE}
SMART_CONFIG_COMPONENTS_O := ${_COMP_O}

SMART_CONFIG_OFILES := ${_EXE}.o ${_COMP_O}
SMART_CONFIG_DEPS   := ${_EXE} ${_SH}

${_COMP_O}: util.h
${_EXE}.o: util.h aslstatus.h config.h

.PHONY: smart-conf
smart-conf: ${SMART_CONFIG_DEPS}
	@echo 'USING SMART CONFIG'
	$(eval export SMART_CONFIG=0)
	$(eval export COMPONENTS=$(addprefix components/,\
		$(call smart_config,./${_SH},./${_EXE},./components)))
	@+$(MAKE)

${_EXE}: ${SMART_CONFIG_OFILES}
	$(CC) -o $@ $^ ${LDFLAGS} ${LDLIBS} ${CFLAGS}

.PHONY: smart-config-clean
smart-config-clean:
	rm -f -- ${SMART_CONFIG_EXE} ${SMART_CONFIG_OFILES}
