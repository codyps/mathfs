#include "eval.h"
#include "error.h"

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

error_t condense_plist(plist_t *pl)
{
	plist_t *pos;
	list_for_each_prev(pos, pl) {
	
	}

	return 0;
}
