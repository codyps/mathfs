#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
typedef int (*op_t)(stack_t **params);
typedef struct {
	char const *name;
	op_t func;
} op_table_t;

int add(stack_t **params)
{
	double x = stack_pop(params);
	double y = stack_pop(params);
	stack_push(params, x + y);
	return 0;
}

int sub(stack_t **params)
{
	double x = stack_pop(params);
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
int tokpath(op_table_t const *ops, stack_t **stack, char const *path)
{
	size_t const  len = strlen(path);
	char   const *end = path + len - 1;

	for (const char *start = end; start >= path; --start) {
		char const *tok;

		// Everything remaining is a token.
		if (start == path)
			tok = start;
		// Ignore the leading slash.
		else if (*start == '/')
			tok = start + 1;
		// Add to existing token.
		else
			continue;

		double value;
		int ret = sscanf(tok, "%lf", &value);

		// Valid double; push it onto the list of parameters.
		if (ret == 1) {
			stack_push(stack, value);
		}
		// If not a double, treat it as a command.
		else {
			op_t op = op_lookup(ops, tok, end);

			if (op) {
				op(stack);
			} else {
				fprintf(stderr, "err: no such function\n");
				return 1;
			}
		}

		end = start - 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	op_table_t ops[] = {
		{ "add", &add },
		{ "sub", &sub },
		{ 0,     0   }
	};
	stack_t *stack = 0;

	int ret = tokpath(ops, &stack, argv[1]);

	while (!stack_isempty(&stack)) {
		double value = stack_pop(&stack);
		if (!ret)
			printf("%f\n", value);
	}
	return ret;
}
