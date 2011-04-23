#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stack_eval.h"


/*
 * Parameter Stack
 */
int stack_isempty(stack_t **head)
{
	return !(*head);
}

void stack_push(stack_t **head, num_t value)
{
	stack_t *node = malloc(sizeof(stack_t));
	node->value = value;
	node->next  = *head;
	*head = node;
}

num_t stack_pop(stack_t **head)
{
	num_t value = (*head)->value;
	stack_t *next = (*head)->next;
	free(*head);
	*head = next;
	return value;
}

static op_t op_lookup(op_table_t const *table, char const *start, const char *end)
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
	char   const *last_slash = strrchr(path, '/');

	if (!strcmp(last_slash + 1, "doc")) {
		/* we need to indicate to the caller that
		 * a doc string needs to be returned.
		 */
	}


	char const *p;
	for (p = last_slash; p >= path; --p) {
		// Decide when to start a new token.
		char const *start;
		if (p == path) {
			start = p;
		} else if (*p == '/') {
			start = p + 1;
		} else {
			continue;
		}

		// All num_ts are parameters; everything else is an operation.
		num_t value;
		int ret = sscanf(start, "%"SCNnum, &value);

		if (ret == 1) {
			stack_push(stack, value);
		} else {
			op_t op = op_lookup(ops, start, end);
			if (!op)
				return ERR_UNDEF_OP;

			error_t err = op(stack);
			if (err)
				return err;
		}
		end = p - 1;
	}
	return 0;
}

