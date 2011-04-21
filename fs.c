
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

static int m_readdir(const char *path, void *buf,
		fuse_fill_dir_t filler, off_t off, struct fuse_file_info *fi)
{
	struct stat stbuf = {
		.st_mode = S_IFDIR | 0755,
		.st_nlink = 2
	};

	filler(buf, ".", &stbuf, off);
	filler(buf, "..", &stbuf, off);


	char const *last_slash = strrchr(path, '/');

	/* TODO: Check if last element in path is in the op table */
		/* TODO: last element is a function, show doc file */
		filler(buf, "doc", &stbuf, off);

	return 0;
}

static int m_open(const char *path, struct fuse_file_info *fi)
{
	/* TODO: fi->fh = contents_of_file */

	char const *last_slash = strrchr(path, '/');



	return 0;
}

/* compliment of m_open */
static int m_release(const char *path, struct fuse_file_info *fi)
{
	/* TODO: free allocations in fi->fh */
	free((void *)fi->fh);

	return 0;
}

static int m_read(const char *path, char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	/* TODO: use fi->fh to copy data into buf */
	return size;
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
