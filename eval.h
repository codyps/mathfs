#ifndef EVAL_H_
#define EVAL_H_

typedef double num_t;
#define PRInum "f"
#define SCNnum "lf"

typedef enum {
ERR_NONE = 0,
ERR_UNDEF_OP,
ERR_TOO_FEW,
ERR_NOT_INT,
} error_t;

char const *error_msg(error_t err);

typedef struct stack_s {
	num_t value;
	struct stack_s *next;
} stack_t;

num_t stack_pop(stack_t **head);
void stack_push(stack_t **head, num_t value);
int stack_isempty(stack_t **head);

typedef error_t (*op_t)(stack_t **params);
typedef struct {
	char const *name;
	char const *doc;
	op_t func;
} op_table_t;

error_t tokpath(op_table_t const *ops, stack_t **stack, char const *path);


#endif

