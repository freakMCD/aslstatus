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
$(call gendeps,thread_helper,thread_helper.h,os.h)
$(call gendeps,util,lib/util.h,${components_config})
$(call gendeps,meminfo,lib/meminfo.h,${util})

util.o: ${util}
libs := $(wildcard lib/*.c)
${libs:.c=.o}: %.o: %.h

# COMPONENTS should be defined

${COMPONENTS:.c=.o}: ${util} ${components_config}

$(call gendeps,battery,components/battery/icons.h,${components_config})
$(wildcard components/battery/*.o): ${battery}

$(call gendeps,cpu,components/cpu/cpu.h,os.h)
components/cpu/linux.o: ${cpu}

$(call gendeps,netspeed,components/netspeed/netspeed.h,os.h)
components/netspeed/linux.o: ${netspeed}

$(wildcard components/volume/*.o): components/volume/volume.h
components/volume/pulse.o: ${thread_helper}

components/ram/linux.o: ${meminfo}

$(call gendeps,aslstatus,aslstatus.h,\
	${util} os.h ${cpu} ${netspeed} components/volume/volume.h)
aslstatus.o: config.h ${thread_helper} ${aslstatus}
