# dependencies

# 1 variable name
# 2 file
# 3 dependencies
#
# example
#   var := somefile.h dep1 dep2
#   somefile.h: dep1 dep2
define _gendeps
$1 := $2 $3
$2: $3
endef
gendeps = $(eval $(call _gendeps,$1,$2,$3))


$(call gendeps,components_config,components_config.h,)
$(call gendeps,thread_helper,thread_helper.h,)
$(call gendeps,util,lib/util.h,${components_config})
$(call gendeps,meminfo,lib/meminfo.h,${util})

util.o: ${util}
libs := $(wildcard lib/*.c)
${libs:.c=.o}: %.o: %.h

# COMPONENTS should be defined

${COMPONENTS:.c=.o}: ${util} ${components_config}

$(call gendeps,battery,components/battery.h,${components_config})
components/battery.o: ${battery}

$(call gendeps,cpu,components/cpu.h,)
components/cpu.o: ${cpu}

$(call gendeps,netspeed,components/netspeed.h,)
components/netspeed.o: ${netspeed}

$(patsubst %.c,%.o,$(wildcard components/volume/*.c)): components/volume.h
components/volume/pulse.o: ${thread_helper}

components/ram.o: ${meminfo}
components/swap.o: ${meminfo}

$(call gendeps,wifi,components/wifi.h,)
components/wifi.o: ${wifi}

$(call gendeps,aslstatus,aslstatus.h,\
	${util}  components/volume.h ${battery} ${cpu} ${netspeed} ${wifi})
aslstatus.o: config.h ${thread_helper} ${aslstatus}
