
#define FUSE_USE_VERSION 26
#include <fuse.h>

#include <string.h>
#include <stdlib.h>
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

int fibs(int opct, double *ops)
{
	return opct;
}

int add(int opct, double *ops)
{
	return opct;
}

int sub(int opct, double *ops)
{
	return opct;
}

struct math_op {
	char *name;
	int (*func)(int opct, double *ops);
} math_ops [] = {
	{ "add",  add },
	{ "fibs", fibs },
	{ "sub",  sub }
};

static int math_op_cmp_by_name(const void *p1, const void *p2)
{
	struct math_op const *m1 = p1, *m2 = p2;
	return strcmp(m1->name, m2->name);
}

struct math_op *math_op_by_name(char *name)
{
	struct math_op tmp = {
		.name = name
	};

	return bsearch(&tmp, math_ops, ARRAY_SIZE(math_ops),
			sizeof(*math_ops), math_op_cmp_by_name);
}

static int m_getattr(const char *path, struct stat *stbuf)
{
	memset(stbuf, 0, sizeof(*stbuf));
	if (!strcmp(path, "/")) {
		stbuf->st_mode  = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}

	return 0;
}



static int m_readdir(const char *path, void *unk,
		fuse_fill_dir_t fd, off_t off, struct fuse_file_info *fi)
{

	return 0;
}

static int m_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

static int m_read(const char *path, char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	return size;
}

static struct fuse_operations math_fsops = {
	.getattr	= m_getattr,
	.open		= m_open,
	.read		= m_read,
	.readdir	= m_readdir
};


int main(int argc, char **argv)
{
	return fuse_main(argc, argv, &math_fsops, NULL);
}
