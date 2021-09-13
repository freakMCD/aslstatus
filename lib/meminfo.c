#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "meminfo.h"

uint8_t
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

uint8_t
read_meminfo_keys(FILE *fptr, const struct meminfo_field *info, size_t size)
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

uint8_t
get_meminfo_ram(FILE *fptr, struct meminfo_ram *info)
{
	const struct meminfo_field fields[] =
	    MEMINFO_INIT_RAM_FIELDS(&info->total,
				    &info->free,
				    &info->available,
				    &info->buffers,
				    &info->cached,
				    &info->shared,
				    &info->reclaimable);

	return read_meminfo_keys(fptr, fields, LEN(fields)) == LEN(fields);
}

uint8_t
get_meminfo_swap(FILE *fptr, struct meminfo_swap *info)
{
	const struct meminfo_field fields[] =
	    MEMINFO_INIT_SWAP_FIELDS(&info->cached, &info->total, &info->free);

	return read_meminfo_keys(fptr, fields, LEN(fields)) == LEN(fields);
}
