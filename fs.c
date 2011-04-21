
#define FUSE_USE_VERSION 26
#include <fuse.h>

#include <string.h>
#include <stdlib.h>
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

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
		fuse_fill_dir_t filler, off_t off, struct fuse_file_info *fi)
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
