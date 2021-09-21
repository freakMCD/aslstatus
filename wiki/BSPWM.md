# bspwm
here is example how to use `aslstatus` with [lemonbar](https://github.com/LemonBoy/bar)

#### config
config with `bspwm` workspaces and colorful components

`config.h`:
```c
/* ... */

static struct arg_t args[] = {

/* function	format				argument	interval (in ms) */

#if USE_X
{ bspwm_ws,	" [ %s ]%%{r}",			NULL,		0,	END },
#endif
{ cpu_perc,	"%%{F#EFA6A2}[c %3s%%]%%{F-}",	NULL,		1000,	END },
{ load_avg,	"-%%{F#74C3E4}[%s]%%{F-}",	NULL,		3000,	END },
{ ram_used,	"-%%{F#C8C874}[m %s]%%{F-} ", 	NULL,		3000,	END },

};

```

#### usage
```sh
aslstatus -s | lemonbar
```


## runit :)
[X/service/bar](https://notabug.org/dm9pZCAq/dotfiles/src/master/.local/X/sv/bar)
