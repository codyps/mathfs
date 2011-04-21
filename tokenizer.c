#include <stdio.h>
#include "eval.h"

static error_t add(stack_t **params)
{
	if (stack_isempty(params)) return ERR_TOO_FEW;
	num_t x = stack_pop(params);
	if (stack_isempty(params)) return ERR_TOO_FEW;
	num_t y = stack_pop(params);

	stack_push(params, x + y);
	return 0;
}

static error_t sub(stack_t **params)
{
	if (stack_isempty(params)) return ERR_TOO_FEW;
	num_t x = stack_pop(params);
	if (stack_isempty(params)) return ERR_TOO_FEW;
	num_t y = stack_pop(params);

	stack_push(params, x - y);
	return 0;
}


int main(int argc, char **argv)
{
	if (argc <= 1) {
		fprintf(stderr, "err: incorrect number of parameters\n");
		fprintf(stderr, "usage: ./mathfs <command>\n");
		return ERR_TOO_FEW;
	}

	op_table_t ops[] = {
		{ "add", &add },
		{ "sub", &sub },
		{ 0,     0   }
	};
	stack_t *stack = 0;

	error_t err = tokpath(ops, &stack, argv[1]);

	// Whatever is left on the stack is the output.
	if (!err) {
		while (!stack_isempty(&stack)) {
			num_t value = stack_pop(&stack);
			printf("%"PRInum"\n", value);
		}
	} else {
		fprintf(stderr, "err: %s\n", error_msg(err));
	}
	return err;
}
