/* See LICENSE file for copyright and license details. */

typedef struct _func_t {
	void (*func)();
	const char name[16];
} func_t;


/* battery */
void battery_perc(char *, const char *);
#define battery_perc {battery_perc, "batt_percentage"}

void battery_state(char *, const char *);
#define battery_state {battery_state, "batt_state"}

void battery_remaining(char *, const char *);
#define battery_remaining {battery_remaining, "batt_remaining"}


/* cpu */
void cpu_freq(char *);
#define cpu_freq {cpu_freq, "cpu_freq"}

void cpu_perc(char *);
#define cpu_perc {cpu_perc, "cpu_percentage"}

/* datetime */
void datetime(char *, const char *);
#define datetime {datetime, "datetime"}


/* disk */
void disk_free(char *, const char *);
#define disk_free {disk_free, "disk_free"}

void disk_perc(char *, const char *);
#define disk_perc {disk_perc, "disk_percentage"}

void disk_total(char *, const char *);
#define disk_total {disk_total, "disk_total"}

void disk_used(char *, const char *);
#define disk_used {disk_used, "disk_used"}


/* entropy */
void entropy(char *);
#define entropy {entropy, "entropy"}


/* hostname */
void hostname(char *);
#define hostname {hostname, "hostname"}


/* ip */
void ipv4(char *, const char *);
#define ipv4 {ipv4, "ipv4"}

void ipv6(char *, const char *);
#define ipv6 {ipv6, "ipv6"}


/* kernel_release */
void kernel_release(char *);
#define kernel_release {kernel_release, "kernel_release"}


/* keyboard_indicators */
void keyboard_indicators(char *, const char *);
#define keyboard_indicators {keyboard_indicators, "kbd_indicators"}


/* keymap */
void keymap(char *);
#define keymap {keymap, "keymap"}


/* load_avg */
void load_avg(char *);
#define load_avg {load_avg, "load_avg"}


/* netspeeds */
void netspeed_rx(char *, const char *, const unsigned int);
#define netspeed_rx {netspeed_rx, "netspeed_rx"}

void netspeed_tx(char *, const char *, const unsigned int);
#define netspeed_tx {netspeed_tx, "netspeed_tx"}


/* num_files */
void num_files(char *, const char *);
#define num_files {num_files, "num_files"}


/* ram */
void ram_free(char *);
#define ram_free {ram_free, "ram_free"}

void ram_perc(char *);
#define ram_perc {ram_perc, "ram_percentage"}

void ram_total(char *);
#define ram_total {ram_total, "ram_total"}

void ram_used(char *);
#define ram_used {ram_used, "ram_used"}


/* run_command */
void run_command(char *, const char *);
#define run_command {run_command, "cmd"}
/*
 * `cmd` thread name hardcodet to be used to run shell commands
 */


/* swap */
void swap_free(char *);
#define swap_free {swap_free, "swap_free"}

void swap_perc(char *);
#define swap_perc {swap_perc, "swap_percentage"}

void swap_total(char *);
#define swap_total {swap_total, "swap_total"}

void swap_used(char *);
#define swap_used {swap_used, "swap_used"}


/* temperature */
void temp(char *, const char *);
#define temp {temp, "temperature"}


/* uptime */
void uptime(char *);
#define uptime {uptime, "uptime"}


/* user */
void gid(char *);
#define gid {gid, "gid"}

void uid(char *);
#define uid {uid, "uid"}

void username(char *);
#define username {username, "username"}


/* volume */
void vol_perc(char *, const char *);
#define vol_perc {vol_perc, "volume"}


/* wifi */
void wifi_perc(char *, const char *);
#define wifi_perc {wifi_perc, "wifi_percentage"}

void wifi_essid(char *, const char *);
#define wifi_essid {wifi_essid, "wifi_essid"}
