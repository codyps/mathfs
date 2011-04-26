
#define FUSE_USE_VERSION 26
#include <fuse.h>

#include <string.h>
#include <stdlib.h>
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#include <errno.h>

#include "parse.h"
#include "eval.h"

/* debug only */
#include <stdio.h>

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


#if 0
static error_t doc(plist_t *pl, plist_t *head)
{
	/* consume all the elements upward. If the directly previous one is
	 * not a TT_OP, error. If there is another doc before this one, error.
	 * If there is anything left following consuming upward, error.
	 */
}
#endif

static op_entry ops [] = {
	OP("add", "addition, anyone?" , add),
	OP("sub", "well, it subtracts", sub),
#if 0
	OP("doc", NULL, doc),
#endif
	{}
};


enum path_type {
	PT_ROOT,
	PT_NORMAL,
	PT_DOC,
	PT_UNK
};

static enum path_type path_type(plist_t *head)
{
	enum path_type type = PT_DOC;

	/* no items */
	if (plist_is_empty(head))
		return PT_ROOT;

	plist_t *pos;

	/* last item is not "doc" */
	if (strcmp(item_entry(head->prev)->raw, "doc")) {
		type = PT_NORMAL;
		pos = head->prev;
		goto upward_check;
	}

	/* only 1 item */
	if (head->prev->prev == head) {
		type = PT_NORMAL;
		pos = head->prev;
		goto upward_check;
	}

	/* function to get doc on */
	item_t *fit = item_entry(head->prev->prev);
	if (fit->type != TT_OP) {
		type = PT_NORMAL;
		pos = head->prev;
		goto upward_check;
	}

	/* we need to skip over the TT_UNK("doc") */
	pos = head->prev->prev;

upward_check:
	for (; pos != head; pos = pos->prev) {
		if (item_entry(pos)->type == TT_UNK)
			type = PT_UNK;
	}

	return type;
}

static int m_getattr(const char *path, struct stat *stbuf)
{
	plist_t pl;
	plist_init(&pl);

	tokpath(ops, &pl, path + 1);

	memset(stbuf, 0, sizeof(*stbuf));

	enum path_type pt = path_type(&pl);
	switch(pt) {
	case PT_NORMAL:
		/* do some extra checking here to see if eval
		 * succeeds. */
		if (eval(&pl)) {
			/* fails, maybe we need more stuff?
			 * Make it a dir. */
			stbuf->st_mode  = S_IFDIR | 0755;
		} else {
			/* succeeds, file it is */
			stbuf->st_mode = S_IFREG | 0644;
		}
		break;

	case PT_ROOT:
		stbuf->st_mode  = S_IFDIR | 0755;
		break;

	case PT_DOC:
		stbuf->st_mode = S_IFREG | 0644;
		break;

	case PT_UNK:
		plist_destroy(&pl);
		return -ENOENT;

	}

	stbuf->st_size  = 4096;
	stbuf->st_blocks = stbuf->st_size / 512;

	plist_destroy(&pl);
	return 0;
}

static int m_readdir(const char *path, void *buf,
		fuse_fill_dir_t filler, off_t off, struct fuse_file_info *fi)
{
	filler(buf, ".", NULL, 0);
	filler(buf, "..",NULL, 0);

	if (!strcmp(path, "/")) {
		/* root dir is special, shows all functions */
		op_entry *op;
		for(op = ops; op->name; op++) {
			if (op->doc)
				filler(buf, op->name, NULL, 0);
		}
	} else {

		plist_t pl;
		plist_init(&pl);
		tokpath(ops, &pl, path + 1);

		/* Check if last element in path is in the op table */
		if (!plist_is_empty(&pl)
				&& item_entry(pl.prev)->type == TT_OP) {
			/* last element is a function, show doc file */
			filler(buf, "doc", NULL, 0);
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
	tokpath(ops, &pl, path + 1);

	enum path_type pt = path_type(&pl);

	switch(pt) {
	case PT_NORMAL: {
		error_t e = eval(&pl);
		if (e) {
			/* IDK */
			return -1;
		}

		int len = plist_to_string(&pl, NULL, 0);

		struct math_fc *fc = malloc(sizeof(*fc) + len);
		fc->len = len;

		plist_to_string(&pl, fc->data, len);

		fi->fh = (intptr_t) fc;
		break;
	}

	case PT_DOC: {
		item_t *op = item_entry(pl.prev->prev);
		int len = strlen(op->op_e->doc);

		struct math_fc *fc = malloc(sizeof(*fc) + len + 1);
		fc->len = len + 1;

		memcpy(fc->data, op->op_e->doc, len);
		fc->data[len] = '\n';

		fi->fh = (intptr_t) fc;
		break;
	}

	case PT_UNK:
	case PT_ROOT:
	default:
		/* shouldn't happen */
		return -1;

	}


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
