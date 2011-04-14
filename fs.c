#include <fuse.h>


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

static struct fuse_operations math_ops = {
	.getattr	= m_getattr,
	.open		= m_open,
	.read		= m_read
	/* XXX: need readdir */
};

int main(int argc, char **argv)
{
	return fuse_main(argc, argv, &math_ops, NULL);
}
