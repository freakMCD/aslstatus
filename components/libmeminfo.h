#ifndef LIBMEMINFO_H
#define LIBMEMINFO_H

#include <err.h>
#include <errno.h>
#include <string.h>

#include "../util.h"

#define MEMINFO_FPTR(F) sysfs_fptr(F, "/", "proc", "meminfo")

/*
 * ERR	error block
 * C	count of matches
 * F	scanf function
 * ...	args to scanf
 */
#define SCANF(ERR, C, F, ...)                                                 \
	do {                                                                  \
		int _m = errno = 0;                                           \
		if ((_m = F(__VA_ARGS__)) == (C)) {                           \
			break;                                                \
		} else if (errno != 0) {                                      \
			warn("%s: " #F, __func__);                            \
			ERR                                                   \
		} else {                                                      \
			warnx("%s: " #F ": matched %d of %d",                 \
			      __func__,                                       \
			      _m,                                             \
			      (C));                                           \
			ERR                                                   \
		}                                                             \
	} while (0)

/* clang-format off */
#define MEMINFO_INIT_FIELDS(T, F, A, B, C, SC, ST, SF, S, R)                  \
	{                                                                     \
		{ WITH_SSIZE("MemTotal"), (T) },                              \
		{ WITH_SSIZE("MemFree"), (F) },                               \
		{ WITH_SSIZE("MemAvailable"), (A) },                          \
		{ WITH_SSIZE("Buffers"), (B) },                               \
		{ WITH_SSIZE("Cached"), (C) },                                \
		{ WITH_SSIZE("SwapCached"), (SC) },                           \
		{ WITH_SSIZE("SwapTotal"), (ST) },                            \
		{ WITH_SSIZE("SwapFree"), (SF) },                             \
		{ WITH_SSIZE("Shmem"), (S) },                                 \
		{ WITH_SSIZE("SReclaimable"), (R) },                          \
	}

#define MEMINFO_INIT                                                          \
	{                                                                     \
		0 /* total */,                                                \
		0 /* free */,                                                 \
		0 /* available */,                                            \
		0 /* buffers */,                                              \
		0 /* cached */,                                               \
		0 /* swap_cached */,                                          \
		0 /* swap_total */,                                           \
		0 /* swap_free */,                                            \
		0 /* shared */,                                               \
		0 /* reclaimable */                                           \
	}
/* clang-format on */

typedef uintmax_t memory_t;

struct meminfo_field {
	const char *key;
	size_t	    key_len;
	memory_t *  value_ptr;
};

struct meminfo {
	/* clang-format off */
	memory_t
		total,
		free,
		available,
		buffers,
		cached,
		swap_cached,
		swap_total,
		swap_free,
		shared,
		reclaimable
	;
	/* clang-format on */
};

static inline uint8_t
read_meminfo(FILE *fptr, const char *key, memory_t *val)
/* read single field from meminfo */
{
	char *	line = NULL;
	size_t	line_len;
	uint8_t found	= 0;
	size_t	key_len = strlen(key);

	while (getline(&line, &line_len, fptr) >= 0) {
		if (!strncmp(line, key, key_len)) {
			SCANF(
			    {
				    free(line);
				    return 0;
			    },
			    1,
			    sscanf,
			    line + key_len + 1,
			    "%ju",
			    val);
			found = !0;
			break;
		}
	}

	free(line);
	return found;
}

static inline uint8_t
read_meminfo_keys(FILE *fptr, const struct meminfo_field *info, size_t size)
/*
 * read multiple fields from meminfo
 *
 * meminfo_field array should be sorted as in /proc/meminfo,
 * because fptr read sequentially and not seeked while reading
 */
{
	char * line = NULL;
	size_t line_len;

	uint8_t readed = 0;

	while (getline(&line, &line_len, fptr) >= 0 && readed < size) {
		if (!strncmp(line, info[readed].key, info[readed].key_len)) {
			SCANF({ goto end_while; },
			      1,
			      sscanf,
			      line + info[readed].key_len + 1,
			      "%ju",
			      info[readed].value_ptr

			);
			readed++;
		}
	end_while:
		continue;
	}

	free(line);
	return readed;
}

static inline uint8_t
get_meminfo(FILE *fptr, struct meminfo *info)
/*
 * get only those fields which needed for `ram` and `swap` components
 */
{
	const struct meminfo_field fields[] =
	    MEMINFO_INIT_FIELDS(&info->total,
				&info->free,
				&info->available,
				&info->buffers,
				&info->cached,
				&info->swap_cached,
				&info->swap_total,
				&info->swap_free,
				&info->shared,
				&info->reclaimable);

	return read_meminfo_keys(fptr, fields, LEN(fields)) == LEN(fields);
}

#endif /* LIBMEMINFO_H */
