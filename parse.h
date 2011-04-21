#ifndef PARSE_H_
#define PARSE_H_

typedef double num_t;
#define PRInum "f"
#define SCNnum "lf"

enum token_type {
	TT_OP,
	TT_NUM,
	TT_DOC,
	TT_UNK
};

typedef error_t (op_fn)(plist_t *);
typedef struct {
	char const *name;
	char const *doc;
	op_fn *func;
} op_entry;

typedef struct plist_s {
	struct item_s *next, *prev;
} plist_t;

typedef struct item_s {
	enum token_type type;

	union {
		struct op_entry *op_e;
		num_t num;
	};
	const char *raw;

	plist_t *pl;
} token_t;


error_t tokpath(op_entry const *ops, plist_t *pl, char const *path);

num_t plist_pop_num(plist_t *pl);
item_t *plist_pop(plist_t *pl);

void plist_push_num(plist_t *pl, num_t num);
void plist_push_op(plist_t *pl, const op_entry *op);
int  plist_push_raw(op_entry const *ops, plist_t *pl, char const *raw, char const *end);

void plist_init(plist_t *pl);
bool plist_is_empty(plist_t *pl);

#define container_of(ptr, type, field) \
	((char *)ptr - offsetof(type, field))

#endif
