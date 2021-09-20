#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "meminfo.h"

#define READLINE_SIZE BUFSIZ

/* clang-format off */
#define MYFILE_INIT(FD) { .fd = (FD), .pos = 0 }
#define MYBUF_INIT      { .pos = 0, .size = 0, .last_nl = 0, .data = {0} }
/* clang-format on */

struct myfile {
	int    fd;
	size_t pos;
};

struct mybuf {
	size_t	pos;
	ssize_t size /* size of valid data */;
	size_t	last_nl /* last '\n' char in 0..size */;
	char	data[READLINE_SIZE];
};

enum read_line_status {
	END = EOF,
	ERR = 0,
	OK  = !0,
};

static enum read_line_status
read_line(struct myfile *file, struct mybuf *line_buf, char **line);

uint8_t
read_meminfo(int fd, const char *key, memory_t *val)
/* read single field from meminfo */
{
	char *	line;
	uint8_t found	   = 0;
	size_t	key_len	   = strlen(key);

	struct mybuf  buf  = MYBUF_INIT;
	struct myfile file = MYFILE_INIT(fd);

	while (read_line(&file, &buf, &line) == OK) {
		if (!strncmp(line, key, key_len)) {
			if (!esscanf(1,
				     line + key_len + 1,
				     memory_t_format,
				     val))
				return 0;

			found = !0;
			break;
		}
	}

	return found;
}

uint8_t
read_meminfo_keys(int fd, const struct meminfo_field *info, size_t size)
{
	char *	      line;
	uint8_t	      readed = 0;
	struct mybuf  buf    = MYBUF_INIT;
	struct myfile file   = MYFILE_INIT(fd);

	while (read_line(&file, &buf, &line) == OK && readed < size) {
		if (!strncmp(line, info[readed].key, info[readed].key_len)) {
			if (!esscanf(1,
				     line + info[readed].key_len + 1,
				     memory_t_format,
				     info[readed].value_ptr))
				goto end_while;

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
read_line(struct myfile *file, struct mybuf *buf, char **line)
{
	__typeof__(buf->size) i;

	if (buf->pos >= buf->last_nl) {
	reread:
		buf->pos = !!buf->pos; /* if not first read, skip last '\n' */

		if (!!buf->last_nl)
			lseek(file->fd,
			      (file->pos -= (buf->size - buf->last_nl)),
			      SEEK_SET);

		if (!eread_ret(buf->size, file->fd, WITH_LEN(buf->data)))
			return ERR;

		if (!buf->size) return END;
		file->pos += buf->size;

		for (i = 0; i < buf->size; i++)
			if (buf->data[i] == '\n') buf->last_nl = i;

		*line = buf->data + buf->pos;
		return OK;
	} else {
		for (; buf->pos <= buf->last_nl; buf->pos++) {
			if (buf->data[buf->pos] == '\n') {
				/* if at last '\n' try to reread next data */
				if (buf->pos == buf->last_nl) goto reread;

				*line = buf->data + ++buf->pos;
				return OK;
			}
		}
	}

	return ERR;
}
