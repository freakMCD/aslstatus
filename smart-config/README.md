# smart-config

`smart-config` used to generate list of needed `C` file

for example if you need only `datetime` in your status,
you whatever compile all components

but with `smart-config` you will compile only needed components


# examples

to manually list needed files, you need:

compile `smart-config`:
```sh
make -C .. smart-config/smart-config
```

run `smart-config.sh`:
```sh
sh ./smart-config.sh ./smart-config ../components
```
* args:
  1. `smart-config` binary
  2. components directory


`smart-config` itself prints names of used functions,
but if function is `vol_perc`,
then it prints `file:` with path to file (e.g. `file:volume/alsa.c`)
