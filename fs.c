
#define FUSE_USE_VERSION 28
#include <fuse.h>

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
	{ "add", add },
	{ "sub", sub },
	{ "fibs", fibs }
};

static int m_getattr(const char *path, struct stat *stbuf)
{
	return 0;
}

/* XXX: need readdir */

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
	.read		= m_read
	/* XXX: need readdir */
};


int main(int argc, char **argv)
{
	return fuse_main(argc, argv, &math_fsops, NULL);
}
