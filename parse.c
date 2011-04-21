#include "parse.h"

static op_entry *op_lookup(const op_entry *table, char const *start, const char *end)
{
	op_entry const *op = table;

	while (op->name && op->func) {
		int len1 = strlen(op->name);
		int len2 = end - start + 1;

		if (len1 == len2 && !memcmp(start, op->name, len1)) {
			return op->func;
		}
		++op;
	}
	return NULL;
}

bool plist_is_empty(plist_t *pl)
{
	return pl->next == pl;
}

void plist_init(plist_t *pl)
{
	pl->next = pl->prev = pl;
}

static void __plist_add(plist_t *new, plist_t *prev, plist_t *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static void plist_add(plist_t *new, plist_t *head)
{
	__plist_add(new, head, head->next);
}

static void plist_add_tail(plist_t *new, plist_t *head)
{
	__plist_add(new, head->prev, head);
}

static void plist_push(plist_t *pl, item_t *it)
{
	plist_add_tail(pl, it->pl);
}

void plist_push_num(plist_t *pl, num_t num)
{
	item_t *it = xmalloc(sizeof(*i));
	memset(it, 0, sizeof(*it));

	it->type = TT_NUM;
	it->num  = num;

	plist_push(pl, it);
}

void plist_push_op(plist_t *pl, const op_entry *op)
{
	item_t *it = xmalloc(sizeof(*i));
	memset(it, 0, sizeof(*it));

	it->type = TT_OP;
	it->op   = op;

	plist_push(pl, it);
}

int plist_push_raw(op_entry const *ops, plist_t *pl, char const *raw, char const *end)
{
	// All num_ts are parameters; everything else is an operation.
	num_t value;
	int ret = sscanf(raw, "%"SCNnum, &value);

	if (ret == 1) {
		plist_push_num(stack, value);
	} else {
		op_entry op = op_lookup(ops, raw, end);
		if (!op) {
			/* TODO: insert some thing indicating an unknown */
			// plist_push_unk(pl, raw, end);
			return -1;
		} else {
			/* TODO: push an op token */
			plist_push_op(pl, op);
		}
	}

	return 0;
}

static void __plist_remove(plist_t *prev, plist_t *next)
{
	next->prev = prev;
	prev->next = next;
}

static void plist_remove(plist_t *entry)
{
	__plist_remove(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

item_t *plist_pop(plist_t *pl)
{
	plist_t *cur = pl->next;

	plist_remove(pl, cur);

	item_t *it = container_of(cur, item_t, pl);
	return it;
}

num_t plist_pop_num(plist_t *pl)
{
	item_t *it = plist_pop(pl);

	num_t n = it->num;
	free(it);
	return n;
}


/*
 * Tokenizer
 */
error_t tokpath(op_entry const *ops, plist_t *pl, char const *path)
{
	size_t const  len = strlen(path);
	char   const *end = path + len - 1;
	char   const *last_slash = end + 1;

	char const *p;
	for (p = end; p >= path; --p) {
		// Decide when to start a new token.
		char const *start;
		if (p == path) {
			start = p;
		} else if (*p == '/') {
			start = p + 1;
		} else {
			continue;
		}

		plist_push_raw(ops, pl, start, last_slash);
		last_slash = start;

		end = p - 1;
	}
	return 0;
}
