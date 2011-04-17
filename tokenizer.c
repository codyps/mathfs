#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum {
ERR_NONE = 0,
ERR_UNDEF_OP,
ERR_TOO_FEW,
ERR_NOT_INT,
} error_t;

char const *error_msg(error_t err)
{
	switch (err) {
	case ERR_NONE:
		return "";
	case ERR_UNDEF_OP:
		return "undefined operation";
	case ERR_TOO_FEW:
		return "too few parameters";
	case ERR_NOT_INT:
		return "parameter must be an integer";
	default:
		return "an unknown error has occured";
	}
}

/*
 * Parameter Stack
 */
typedef struct stack_s {
	double value;
	struct stack_s *next;
} stack_t;

int stack_isempty(stack_t **head)
{
	return !(*head);
}

void stack_push(stack_t **head, double value)
{
	stack_t *node = malloc(sizeof(stack_t));
	node->value = value;
	node->next  = *head;
	*head = node;
}

double stack_pop(stack_t **head)
{
	double value = (*head)->value;
	stack_t *next = (*head)->next;
	free(*head);
	*head = next;
	return value;
}

/*
 * Supported Operations
 */
typedef error_t (*op_t)(stack_t **params);
typedef struct {
	char const *name;
	op_t func;
} op_table_t;

error_t add(stack_t **params)
{
	if (stack_isempty(params)) return ERR_TOO_FEW;
	double x = stack_pop(params);
	if (stack_isempty(params)) return ERR_TOO_FEW;
	double y = stack_pop(params);

	stack_push(params, x + y);
	return 0;
}

error_t sub(stack_t **params)
{
	if (stack_isempty(params)) return ERR_TOO_FEW;
	double x = stack_pop(params);
	if (stack_isempty(params)) return ERR_TOO_FEW;
	double y = stack_pop(params);

	stack_push(params, x - y);
	return 0;
}

op_t op_lookup(op_table_t const *table, char const *start, const char *end)
{
	op_table_t const *op = table;

	while (op->name && op->func) {
		int len1 = strlen(op->name);
		int len2 = end - start + 1;

		if (len1 == len2 && !memcmp(start, op->name, len1)) {
			return op->func;
		}
		++op;
	}
	return 0;
}

/*
 * Tokenizer
 */
error_t tokpath(op_table_t const *ops, stack_t **stack, char const *path)
{
	size_t const  len = strlen(path);
	char   const *end = path + len - 1;

	for (const char *p = end; p >= path; --p) {
		// Decide when to start a new token.
		char const *start;
		if (p == path) {
			start = p;
		} else if (*p == '/') {
			start = p + 1;
		} else {
			continue;
		}

		// All doubles are parameters; everything else is an operation.
		double value;
		int ret = sscanf(start, "%lf", &value);

		if (ret == 1) {
			stack_push(stack, value);
		} else {
			op_t op = op_lookup(ops, start, end);
			if (!op) return ERR_UNDEF_OP;

			error_t err = op(stack);
			if (err) return err;
		}
		end = p - 1;
	}
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
			double value = stack_pop(&stack);
			printf("%f\n", value);
		}
	} else {
		fprintf(stderr, "err: %s\n", error_msg(err));
	}
	return err;
}
