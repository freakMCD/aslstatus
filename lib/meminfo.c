#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "meminfo.h"

/*
 * from linux sources: fs/proc/meminfo.c
 *
 * 16	key: (with spaces)
 * 8	%8lu
 * 4	" kB\n"
 */
#define LINE_LEN     (16 + 8 + 4)
#define MEMINFO_LINE (LINE_LEN + LINE_LEN / 2)

struct myfile {
	int    fd;
	size_t pos;
};
enum read_line_status {
	END = EOF,
	ERR = 0,
	OK  = !0,
};

static enum read_line_status read_line(struct myfile *file,
				       char	      line[MEMINFO_LINE]);

uint8_t
read_meminfo(int fd, const char *key, memory_t *val)
/* read single field from meminfo */
{
	uint8_t	      found = 0;
	char	      line[MEMINFO_LINE];
	size_t	      key_len = strlen(key);
	struct myfile file    = { .fd = fd, .pos = 0 };

	while (read_line(&file, line) == OK) {
		if (!strncmp(line, key, key_len)) {
			SCANF({ return 0; },
			      1,
			      sscanf,
			      line + key_len + 1,
			      memory_t_format,
			      val);
			found = !0;
			break;
		}
	}

	return found;
}

uint8_t
read_meminfo_keys(int fd, const struct meminfo_field *info, size_t size)
{
	char  line[MEMINFO_LINE];
	uint8_t readed = 0;
	struct myfile file = { .fd = fd, .pos = 0 };

	while (read_line(&file, line) == OK && readed < size) {
		if (!strncmp(line, info[readed].key, info[readed].key_len)) {
			SCANF({ goto end_while; },
			      1,
			      sscanf,
			      line + info[readed].key_len + 1,
			      memory_t_format,
			      info[readed].value_ptr

			);
			readed++;
		}
	end_while:
		continue;
	}

	return readed;
}

uint8_t
get_meminfo_ram(int fd, struct meminfo_ram *info)
{
	const struct meminfo_field fields[] =
	    MEMINFO_INIT_RAM_FIELDS(&info->total,
				    &info->free,
				    &info->available,
				    &info->buffers,
				    &info->cached,
				    &info->shared,
				    &info->reclaimable);

	return read_meminfo_keys(fd, fields, LEN(fields)) == LEN(fields);
}

uint8_t
get_meminfo_swap(int fd, struct meminfo_swap *info)
{
	const struct meminfo_field fields[] =
	    MEMINFO_INIT_SWAP_FIELDS(&info->cached, &info->total, &info->free);

	return read_meminfo_keys(fd, fields, LEN(fields)) == LEN(fields);
}

static inline enum read_line_status
read_line(struct myfile *file, char line[MEMINFO_LINE])
{
	/*
	 * TODO
	 * 
	 * rewrite with less syscalls and larger buffer
	 */
	size_t readed, i;
	EREAD({ return ERR; }, readed, file->fd, line, MEMINFO_LINE);

	if (!readed) return END;
	for (i = 0; i < readed; i++)
		if (line[i] == '\n') {
			line[i] = '\0';
			file->pos += i + 1;
			lseek(file->fd, file->pos, SEEK_SET);
			return OK;
		}

	return ERR;
}
