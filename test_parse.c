#include "parse.h"
#include "error.h"
#include "eval.h"
#include "op.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	error_t err;

	if (argc <= 1) {
		fprintf(stderr, "err: incorrect number of parameters\n");
		fprintf(stderr, "usage: ./%s <command>\n", argc?argv[0]:"mathfs");
		return ERR_TOO_FEW;
	}

	plist_t pd;
	plist_init(&pd);

	tokpath(ops, &pd, argv[1]);

	err = eval(&pd);
	if (err)
		goto done;

	size_t blen = plist_to_string(&pd, NULL, 0);

	char *buf = malloc(blen + 1);

	int extra = plist_to_string(&pd, buf, blen + 1);
	printf("%s", buf);

done:
	if (extra < 0) {
		fprintf(stderr, "err: %s\n", error_msg(err));
	}
	return err;
}
