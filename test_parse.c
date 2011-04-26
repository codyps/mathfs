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

	err = tokpath(ops, &pd, argv[1]);
	if (err) goto done;

	err = eval(&pd);
	if (err) goto done;

	char buf[2048];
	int extra = plist_to_string(&pd, buf, sizeof(buf));
	printf("%s", buf);

done:
	if (extra < 0) {
		fprintf(stderr, "err: %s\n", error_msg(err));
	}
	return err;
}
