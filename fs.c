
#define FUSE_USE_VERSION 26
#include <fuse.h>

#include <string.h>
#include <stdlib.h>
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#include "parse.h"
#include "eval.h"

static error_t add(plist_t *pl, plist_t *head)
{
	num_t x, y;
	error_t err;

	err = plist_pop_num(pl, head, &x);
	if (err) return err;
	err = plist_pop_num(pl, head, &y);
	if (err) return err;

	plist_push_num(pl, x + y);
	return 0;
}

static error_t sub(plist_t *pl, plist_t *head)
{
	num_t x, y;
	error_t err;

	err = plist_pop_num(pl, head, &x);
	if (err) return err;
	err = plist_pop_num(pl, head, &y);
	if (err) return err;

	plist_push_num(pl, x - y);
	return 0;
}

op_entry ops [] = {
	OP("add", "addition, anyone?" , add),
	OP("sub", "well, it subtracts", sub),
	{}
};

static int m_getattr(const char *path, struct stat *stbuf)
{
	memset(stbuf, 0, sizeof(*stbuf));
	stbuf->st_mode  = S_IFDIR | 0755;
	stbuf->st_nlink = 2;

	return 0;
}

static int m_readdir(const char *path, void *buf,
		fuse_fill_dir_t filler, off_t off, struct fuse_file_info *fi)
{
	struct stat stbuf = {
		.st_mode = S_IFDIR | 0755,
		.st_nlink = 2
	};

	filler(buf, ".", &stbuf, off);
	filler(buf, "..", &stbuf, off);

	if (!strcmp(path, "/")) {
		/* root dir is special, shows all functions */
		op_entry *op;
		for(op = ops; op->name; op++) {
			filler(buf, op->name, &stbuf, off);
		}
	} else {

		plist_t pl;
		plist_init(&pl);
		error_t r = tokpath(ops, &pl, path);
		if (r)
			return -1;

		/* Check if last element in path is in the op table */
		if (!plist_is_empty(&pl) && item_entry(pl.prev)->type == TT_OP) {
			/* last element is a function, show doc file */
			filler(buf, "doc", &stbuf, off);
		}

		plist_destroy(&pl);

	}

	return 0;
}


struct math_fc {
	size_t len;
	char data[];
};

static int m_open(const char *path, struct fuse_file_info *fi)
{
	/* fi->fh = contents_of_file */

	plist_t pl;
	plist_init(&pl);
	error_t r = tokpath(ops, &pl, path);
	if (r)
		return -1;

	error_t e = eval(&pl);
	if (e) {
		/* IDK */
	}

	int len = plist_to_string(&pl, NULL, 0);

	struct math_fc *fc = malloc(sizeof(*fc) + len);
	fc->len = len;

	plist_to_string(&pl, fc->data, len);

	fi->fh = (intptr_t) fc;

	plist_destroy(&pl);

	return 0;
}

static inline struct math_fc *fh_to_math_fc(uint64_t fh) {
	return (struct math_fc *)((intptr_t)fh);
}

/* compliment of m_open */
static int m_release(const char *path, struct fuse_file_info *fi)
{
	/* free allocations in fi->fh */
	free(fh_to_math_fc(fi->fh));

	return 0;
}

#define MIN(x, y) ((x) < (y)?(x):(y))

static int m_read(const char *path, char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	/* use fi->fh to copy data into buf */

	struct math_fc *fc = fh_to_math_fc(fi->fh);

	size_t cpy_len = MIN(fc->len - offset, size);
	memcpy(buf, fc->data + offset, cpy_len);

	return cpy_len;
}


static struct fuse_operations math_fsops = {
	.getattr	= m_getattr,
	.open		= m_open,
	.release	= m_release,
	.read		= m_read,
	.readdir	= m_readdir
};


int main(int argc, char **argv)
{
	return fuse_main(argc, argv, &math_fsops, NULL);
}
