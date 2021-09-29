% aslstatus(1)
% dm9pZCAq


# NAME

`aslstatus` — async suckless status monitor


# SYNOPSIS

aslstatus [*options*]


# DESCRIPTION

**aslstatus** is a sucklessmulti thread status monitor for window managers
that use *WM_NAME* (e.g. **dwm**) or *stdin* (e.g **lemonbar**) to fill the status bar.
By defaul if compiled with *X=1*, outputs to *WM_NAME*,


# OPTIONS

`-h`

: Show help

`-v`

: Show version

`-d`

: Dump config to stdout

`-s`

: If compiled with *X=1*, outputs to *stdout*


# SIGNALS

If segment (thread) receive *USR1* signal then that segmen will be force updated


# CUSTOMIZATION
**aslstatus** can be customized by creating a custom *config.h*
and *components_config.h* and (re)compiling the source code.
This keeps it fast, secure and simple.

# BUGS

Bugs can be reported and filed at https://notabug.org/dm9pZCAq/aslstatus/issues

