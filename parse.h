#ifndef PARSE_H_
#define PARSE_H_

#include "error.h"
#include <stdlib.h> /* size_t */
#include <stdbool.h>
#include <stddef.h> /* offsetof */

#ifdef NUM_FLOAT
typedef double num_t;
#define PRInum "f"
#define SCNnum "lf"
#else
typedef long num_t;
#define PRInum "ld"
#define SCNnum "ld"
#endif

enum token_type {
	TT_OP,
	TT_NUM,
	TT_UNK
};

#define plist_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define plist_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

typedef struct plist_s {
	struct plist_s *prev, *next;
} plist_t;

typedef error_t (op_fn)(plist_t *pl, plist_t *head);
typedef struct {
	char const *name;
	char const *doc;
	op_fn *func;
} op_entry;


#define OP(name, doc, func) { name, doc, func }

typedef struct item_s {
	enum token_type type;

	union {
		op_entry const *op_e;
		num_t num;
	};

	char *raw;
	size_t      raw_len;

	plist_t pl;
} item_t;

error_t tokpath(op_entry const *ops, plist_t *pl, char const *path);

error_t plist_pop_num(plist_t *pl, plist_t *head, num_t *num);
item_t *plist_pop(plist_t *pl);

void plist_push(plist_t *pl, item_t *it);
void plist_push_num(plist_t *pl, num_t num);
void plist_push_op(plist_t *pl, op_entry const *op);
void plist_push_unk(plist_t *pl, char const *start, char const *end);

void    plist_push_raw(op_entry const *ops, plist_t *pl, char const *raw, char const *end);
item_t *item_mk(op_entry const *ops, char const *raw, char const *end);
void    item_destroy(item_t *it);

int item_to_string(item_t const *it, char *buf, size_t len);
int plist_to_string(plist_t const *pl, char *buf, size_t len);

void plist_init(plist_t *pl);
bool plist_is_empty(plist_t *pl);
void plist_destroy(plist_t *pl);

#define item_entry(ptr) container_of(ptr, item_t, pl)
#define container_of(ptr, type, field) \
	((type *)((char *)ptr - offsetof(type, field)))

#endif
