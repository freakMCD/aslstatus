#!/bin/sh --
set -ue

EXE="${1:?}"
COMPONENTS="${2:?}"

find_file() {
	found=0
	func="${1:?}"
	pattern="s/.*\(${func}\)\s*([^)]\+)\s*{.*/\1/p"

	for file in "${COMPONENTS}"/*.c; do
		[ -z "$(sed -zn "${pattern}" "${file}")" ] || {
			found=1
			echo "${file##*/}"
		}
	done
	[ "${found}" -eq 1 ] || 1>&2 echo "${func} not found"
}


for func in $("${EXE}" | sort | uniq); do
	case "${func}" in
	file:*) echo "${func#file:}";;
	*) find_file "${func}";;
	esac
done | sort | uniq
