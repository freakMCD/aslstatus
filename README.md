# ASLSTATUS

---
if you found any issues report it here

for all other questions, feel free to ask in matrix
[#aslstatus:matrix.org](https://matrix.to/#/#aslstatus:matrix.org)

---
![demo](https://i.ibb.co/r5PYRj8/Demo.gif)

---
# instalation
### Gentoo
avaliable in [my overlay](https://notabug.org/dm9pZCAq/dm9pZCAq-overlay):
```sh
eselect repository enable dm9pZCAq
emerge --sync dm9pZCAq
emerge app-admin/aslstatus::dm9pZCAq
```

if you wanna pack `aslstatus` for you distro,
please do so and submit PR to add it here

---
# compile

it is successfully compiling with
* `clang`
* `tcc`
* `gcc`

needs `pkg-config` for `LDLIBS`

you can find out more in [config.mk](config.mk)

## make options
#### X
add support for `X` (needed for `dwm` `WM_NAME`)

you may want to set it to `0` if you use aslstatus in [dvtm](https://www.brain-dump.org/projects/dvtm/)

* default: `1`
* dependencies:
  + `libxcb-dev`

#### XKB
add support for keyboard (needed for `keymap` component)

works only if `X=1`

* default: `1`
* dependencies:
  + `libxcb-xkb-dev`

#### AUDIO
select audio library

* default: `ALSA`
* possible values:
  - `ALSA`
  - `PULSE`
  - `''` (empty) to use `sys/soundcard.h` or `sys/audioio.h` on OpenBSD
* dependencies:
  + `libasound-dev` (if `AUDIO=ALSA`)
  + `libpulse-dev` (if `AUDIO=PULSE`)


---
# config

for more info about components see [wiki](wiki/COMPONENTS.md) and [components_config.h](components_config.h)

---
see also [main wiki page](wiki/), you can find some useful stuff here
