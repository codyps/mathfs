/*
 * Cody Schafer <cpschafer@gmail.com>
 * Michael Koval <koval.michael@gmail.com>
 */

#include "parse.h"
#include <string.h>
#include <stdio.h> /* sscanf */

static op_entry const *op_lookup(op_entry const *table,
		char const *start, const char *end)
{
	op_entry const *op = table;

	while (op->name && op->func) {
		int len1 = strlen(op->name);
		int len2 = end - start;

		if (len1 == len2 && !memcmp(start, op->name, len1)) {
			return op;
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

static void plist_add_tail(plist_t *new, plist_t *head)
{
	__plist_add(new, head->prev, head);
}

void plist_push(plist_t *pl, item_t *it)
{
	plist_add_tail(&it->pl, pl);
}

void plist_push_num(plist_t *pl, num_t num)
{
	item_t *it = malloc(sizeof(*it));
	memset(it, 0, sizeof(*it));

	it->type = TT_NUM;
	it->num  = num;

	plist_push(pl, it);
}

void plist_push_op(plist_t *pl, const op_entry *op)
{
	item_t *it = malloc(sizeof(*it));
	memset(it, 0, sizeof(*it));

	it->type = TT_OP;
	it->op_e = op;

	plist_push(pl, it);
}

void plist_push_unk(plist_t *pl, char const *start, char const *end)
{
	item_t *it = malloc(sizeof(*it));
	memset(it, 0, sizeof(*it));

	it->type = TT_UNK;
	size_t len = end - start;
	it->raw  = malloc(len + 1);
	memcpy(it->raw, start, len);
	it->raw[len] = 0;

	plist_push(pl, it);
}

void plist_push_raw(op_entry const *ops, plist_t *pl, char const *raw, char const *end)
{
	item_t *it = item_mk(ops, raw, end);
	plist_push(pl, it);
}

void item_destroy(item_t *it)
{
	free(it->raw);
	free(it);
}

item_t *item_mk(op_entry const *ops, char const *start, char const *end)
{
	// All num_ts are parameters; everything else is an operation.
	num_t value;

	item_t *it = malloc(sizeof(*it));
	memset(it, 0, sizeof(*it));

	size_t len = end - start;
	it->raw  = malloc(len + 1);
	memcpy(it->raw, start, len);
	it->raw[len] = 0;
	it->raw_len = len;

	int ret = sscanf(start, "%"SCNnum, &value);

	if (ret == 1) {
		it->type = TT_NUM;
		it->num  = value;
	} else {
		op_entry const *op = op_lookup(ops, start, end);
		if (!op) {
			/* insert some thing indicating an unknown */
			it->type = TT_UNK;
		} else {
			/* push an op token */
			it->op_e = op;
			it->type = TT_OP;
		}
	}

	return it;
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

	plist_remove(cur);

	item_t *it = container_of(cur, item_t, pl);
	return it;
}

error_t plist_pop_num(plist_t *pl, plist_t *head, num_t *num)
{
	/* no arguments in front of pl to pop */
	if (pl->next == head) {
		return ERR_TOO_FEW;
	}

	item_t *it = plist_pop(pl);

	if (it->type == TT_NUM) {
		*num = it->num;
		item_destroy(it);
		return ERR_NONE;
	} else {
		item_destroy(it);
		return ERR_NOT_INT;
	}
}

void plist_destroy(plist_t *head)
{
	/* free all items (not head, it isn't in an item) */
	plist_t *pos;
	plist_t tmp;
	plist_for_each(pos, head) {
		item_t *it = item_entry(pos);

		/* this prevents a use after free */
		tmp = it->pl;
		pos = &tmp;

		item_destroy(it);
	}
}

int item_to_string(item_t const *it, char *buf, size_t len)
{
	switch(it->type) {
	case TT_OP:
		return snprintf(buf, len, "%s\n", it->op_e->name);

	case TT_NUM:
		return snprintf(buf, len, "%"PRInum"\n", it->num);

	case TT_UNK:
		return snprintf(buf, len, "%s\n", it->raw);

	default:
		return -1;

	}
}

#define MAX(x,y) ((x) > (y)?(x):(y))

int plist_to_string(plist_t const *head, char *buf, size_t len)
{
	int consumed = 0;
	size_t orig_len = len;

	plist_t *pos;
	plist_for_each(pos, head) {
		item_t *it = item_entry(pos);

		int n = item_to_string(it, buf + consumed, len);

		if (n < 0) {
			return n;
		}

		consumed += n;
		if (len && len > n) {
			len -= n;
		} else {
			len = 0;
		}
	}
	if (orig_len)
		buf[orig_len-1] = '\0';
	return consumed;
}

/*
 * Tokenizer
 */
void tokpath(op_entry const *ops, plist_t *pl, char const *path)
{
	char const *p;
	char const *tok_start = path;
	for (p = path;; p++) {
		if (*p != '/' && *p != '\0')
			continue;

		if (tok_start < p) {
			plist_push_raw(ops, pl, tok_start, p);
			tok_start = p + 1;
		}

		if (*p == '\0')
			break;
	}
}
