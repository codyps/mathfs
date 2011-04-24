#include "parse.h"
#include "error.h"
#include "eval.h"
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


int main(int argc, char **argv)
{
	error_t err;

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

	err = tokpath(ops, &pd, argv[1]);
	if (err) goto done;

	err = eval(&pd);
	if (err) goto done;

	char buf[100];
	int extra = plist_to_string(&pd, buf, sizeof buf);
	printf("%s\n", buf);

	// Whatever is left on the stack is the output.
	while (!plist_is_empty(&pd)) {
		item_t *it = plist_pop(&pd);

		char buf[2048];
		item_to_string(it, buf, sizeof(buf));
		printf("%s\n", buf);
	}

done:
	if (err) {
		fprintf(stderr, "err: %s\n", error_msg(err));
	}
	return err;
}
