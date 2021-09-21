# battery
* arg: battery name
  * example: `BAT0`
* info from `/sys/class/power_supply/${arg}`

## battery_perc
**battery percentage**
* info from `/sys/class/power_supply/${arg}/capacity`

## battery_state
**battery charging state**
* can be configured from [components_config.h](#components_config)
* info from `/sys/class/power_supply/${arg}/status`

## battery_remaining
**battery remaining HH:MM**
* can be configured from [components_config.h](#components_config)

---

# brightness
**backlight display brightness**
* arg: device name
  * example: `intel_backlight`
* info from `/sys/class/backlight/${arg}/{max_,}brightness`

---

# bspwm_ws
**[bspwm](https://github.com/baskerville/bspwm) workspaces (for lemonbar?)**
* arg: `NULL`
* interval: can be `0`
* read events from `bspwm` socket
* can be configured from [components_config.h](#components_config)
<!-- TODO: more info about bspwm -->

---

# cpu
* arg: `NULL`
* linux:
  * reads info from first line of `/proc/stat` (see: `man 5 proc`)

## cpu_perc
**cpu usage in percent**
  
## cpu_freq
**cpu frequency**

---

# datetime
**date and time**
* arg: datetime format (see: `man 3 strftime`)
  * example: `%F %T`

---

# disk
* arg: mountpoint (see: `man 8 findmnt`)
  * example: `/`, `/home`, `/media`, ...
* reads info from `statfs` syscall (see: `man 2 statfs` and `man 3 statvfs`)

## disk_free
**free disk space**

## disk_perc
**disk usage in percent**

## disk_total
**total disk space**

## disk_used
**used disk space**

---

# entropy
**available entropy**
* arg: `NULL`
* bsd: 
  * always infinity
  * can be configured from [components_config.h](#components_config)
* linux:
  * from: `/proc/sys/kernel/random/entropy_avail`

---

# hostname
**name of current host**
* arg: `NULL`
* interval: can be `ONCE`
* reads info from `uname` syscall (see: `man 2 uname` and `man 3 gethostname`)

---

# ip
* arg: interface name
  * example: `eth0`

## ipv4
**IPv4 address**

## ipv6
**IPv6 address**

---

# kernel_release
**operating system release level**
* arg: `NULL`
* same as `uname -r`
* reads info from `uname` syscall (see: `man 2 uname`)

---

# keymap
**layout of current keymap**
* arg: `NULL`
* interval: can be `0`
* can be configured from [components_config.h](#components_config)
* read events from `Xorg`
* needs to be build with `X=1` and `XKB=1`
<!-- TODO: add more info -->

---

# load_avg
**load average**
* arg: `NULL`
* in format: `%.2f %.2f %.2f`
* reads info from `sysinfo` syscall (see: `man 2 sysinfo` and `man 3 getloadavg`)

---

# netspeed
* number of bytes transmitted per `interval`
* arg: interface name
  * example: `wlan0`
* info from `/sys/class/net/${arg}/statistics/{r,t}x_bytes`

## netspeed_rx
**receive network speed**

## netspeed_tx
**transfer network speed**

---

# num_files
**number of files in a directory**
* arg: path
  * example: `/home/user/Mail/Inbox/cur`

---

# memory info
* `ram` and `swap`
* arg: `NULL`
* info from `/proc/meminfo`
  * same as in `htop`, `top`, `free`

## ram_free
**free memory**

## ram_perc
**memory usage in percent**

## ram_total
**total memory size**
* info from `sysinfo` syscall (see: `man 2 sysinfo`)

## ram_used
**used memory**

## swap_free
**free swap**

## swap_perc
**swap usage in percent**

## swap_total
**total swap size**
* info from `sysinfo` syscall (see: `man 2 sysinfo`)

## swap_used
**used swap**

---

# run_command
**display output of custom shell command**
* arg: command
  * example: `curl -s -- 'wttr.in/?format=1'`
* see: `man 3 popen`

---

# temp
**temperature in degree celsius**
* arg: `NULL` or thermal zone
  * example: `hwmon0`, `hwmon1`, ...
* reads info from:
  * if arg: `/sys/class/hwmon/${arg}/temp1_input`
  * if `NULL`: will automatically find a suitable zone

---

# uptime
**system uptime**
* arg: `NULL`
* reads info from `clock_gettime` (see: `man 2 clock_gettime`)

---

# user
* arg: `NULL`
* interval: can be `ONCE`

## uid
**UID of current user**

## gid
**GID of current user**

## username
**username of current user**

<!-- add more info -->

---

# vol_perc
**volume in percent**
* alsa:
  * needs to be build with `AUDIO=ALSA`
  * arg: `NULL` (will be used default mixer)
* pulse: 
  * needs to be build with `AUDIO=PULSE`
  * arg: `NULL` (will be used default ?mixer?)
* default:
  * arg: mixer file
    * example: `/dev/mixer`
  * linux (and freebsd?): `sys/soundcard.h`

---

# wifi
* arg: interface name
  * example: `wlan0`
* reads info from `ioctl` syscall (see: `man 2 ioctl` and `linux/wireguard.h`)

## wifi_perc
**WiFi signal in percent**
* from: `SIOCGIWSTATS` `ioctl`

## wifi_essid
**WiFi ESSID**
* from: `SIOCGIWESSID` `ioctl`

---
