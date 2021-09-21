#ifndef MEMINFO_H
#define MEMINFO_H

#include <stdio.h>
#include <inttypes.h>

#include "util.h"

#define MEMINFO_FD(fd_ptr) sysfs_fd_or_rewind((fd_ptr), "/", "proc", "meminfo")

/* clang-format off */
#define MEMINFO_INIT_RAM_FIELDS(T, F, A, B, C, S, R)                          \
	{                                                                     \
		{ WITH_SSIZE("MemTotal"), (T) },                              \
		{ WITH_SSIZE("MemFree"), (F) },                               \
		{ WITH_SSIZE("MemAvailable"), (A) },                          \
		{ WITH_SSIZE("Buffers"), (B) },                               \
		{ WITH_SSIZE("Cached"), (C) },                                \
		{ WITH_SSIZE("Shmem"), (S) },                                 \
		{ WITH_SSIZE("SReclaimable"), (R) },                          \
	}

#define MEMINFO_INIT_SWAP_FIELDS(C, T, F)                                     \
	{                                                                     \
		{ WITH_SSIZE("SwapCached"), (C) },                            \
		{ WITH_SSIZE("SwapTotal"), (T) },                             \
		{ WITH_SSIZE("SwapFree"), (F) },                              \
	}

#define MEMINFO_INIT_RAM                                                      \
	{                                                                     \
		0 /* total */,                                                \
		0 /* free */,                                                 \
		0 /* available */,                                            \
		0 /* buffers */,                                              \
		0 /* cached */,                                               \
		0 /* shared */,                                               \
		0 /* reclaimable */                                           \
	}

#define MEMINFO_INIT_SWAP                                                     \
	{                                                                     \
		0 /* cached */,                                               \
		0 /* total */,                                                \
		0 /* free */,                                                 \
	}
/* clang-format on */

#define memory_t_format "%8" PRIu64
typedef uint64_t memory_t;

struct meminfo_field {
	const char *key;
	size_t	    key_len;
	memory_t *  value_ptr;
};

struct meminfo_ram {
	/* clang-format off */
	memory_t
		total,
		free,
		available,
		buffers,
		cached,
		shared,
		reclaimable
	;
	/* clang-format on */
};

struct meminfo_swap {
	memory_t cached, total, free;
};

/* read single field from meminfo */
uint8_t read_meminfo(int fd, const char *key, memory_t *val);

/*
 * read multiple fields from meminfo
 *
 * meminfo_field array should be sorted as in /proc/meminfo,
 * because fptr read sequentially and not seeked while reading
 */
uint8_t
read_meminfo_keys(int fd, const struct meminfo_field *info, size_t size);

/* get only those fields which needed for `ram` components */
uint8_t get_meminfo_ram(int fd, struct meminfo_ram *info);

/* get only those fields which needed for `swap` components */
uint8_t get_meminfo_swap(int fd, struct meminfo_swap *info);

#endif /* MEMINFO_H */
