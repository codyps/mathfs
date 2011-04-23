#include "parse.h"
#include "error.h"
#include <stdio.h>

static error_t add(plist_t *pl)
{
	if (plist_is_empty(pl))
		return ERR_TOO_FEW;
	num_t x = plist_pop_num(pl);

	if (plist_is_empty(pl))
		return ERR_TOO_FEW;
	num_t y = plist_pop_num(pl);

	plist_push_num(pl, x + y);
	return 0;
}

static error_t sub(plist_t *pl)
{
	if (plist_is_empty(pl))
		return ERR_TOO_FEW;
	num_t x = plist_pop_num(pl);

	if (plist_is_empty(pl))
		return ERR_TOO_FEW;
	num_t y = plist_pop_num(pl);

	plist_push_num(pl, x - y);
	return 0;
}


int main(int argc, char **argv)
{
	if (argc <= 1) {
		fprintf(stderr, "err: incorrect number of parameters\n");
		fprintf(stderr, "usage: ./%s <command>\n", argc?argv[0]:"mathfs");
		return ERR_TOO_FEW;
	}

	op_entry ops[] = {
		{ "add", "adds 2 numbers", add },
		{ "sub", "subtract 2 numbers", sub },
		{}
	};

	plist_t pd;
	plist_init(&pd);

	error_t err = tokpath(ops, &pd, argv[1]);

	// Whatever is left on the stack is the output.
	if (!err) {
		while (!plist_is_empty(&pd)) {
			item_t *it = plist_pop(&pd);

			char buf[2048];
			item_to_string(it, buf, sizeof(buf));

			printf("%s\n", buf);

		}
	} else {
		fprintf(stderr, "err: %s\n", error_msg(err));
	}
	return err;
}
