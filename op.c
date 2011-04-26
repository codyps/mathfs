#include "op.h"
#include <math.h>
#include <stdio.h> /* debug only */

#define ABS(_x_)      (((_x_) >= 0) ? (_x_) : -(_x_))
#define POW(_x_, _y_) ((num_t)pow((_x_), (_y_)))
#ifdef NUM_FLOAT
#define MOD(_x_, _y_) (fmod((_x_), (_y_)))
#else
#define MOD(_x_, _y_) ((_x_) % (_y_))
#endif

static bool is_prime(num_t n)
{
	num_t i;
	for (i = 2; i < n; ++i) {
		if (i != n && MOD(n, i) == 0) {
			return false;
		}
	}
	return true;
}

static error_t fs_factor(plist_t *pl, plist_t *head)
{
	num_t n;
	error_t err;

	err = plist_pop_num(pl, head, &n);
	if (err) return err;

	/* "Factor" of -1 is a unit, so it is not a factor. */
	n = ABS(n);
	num_t i;

	for (i = 2; i <= n; ++i) {
		if (is_prime(i) && MOD(n, i) == 0) {
			plist_push_num(pl, i);
		}
	}
	return 0;
}

static error_t fs_fib(plist_t *pl, plist_t *head)
{
	num_t n;
	error_t err;

	err = plist_pop_num(pl, head, &n);
	if (err) {
		return err;
	} else if (n < 1) {
		return ERR_NOT_VALID;
	}

	num_t a = 1;
	num_t b = 0;
	num_t c;
	num_t i;

	for (i = 0; i < n; ++i) {
		plist_push_num(pl, a);
		c = a + b;
		b = a;
		a = c;
	}
	return 0;
}

static error_t fs_add(plist_t *pl, plist_t *head)
{
	num_t x, y;
	error_t err;

	err = plist_pop_num(pl, head, &x);
	if (err) return err;
	err = plist_pop_num(pl, head, &y);
	if (err) return err;

	plist_push_num(pl, x + y);
	return 0;
}

static error_t fs_sub(plist_t *pl, plist_t *head)
{
	num_t x, y;
	error_t err;

	err = plist_pop_num(pl, head, &x);
	if (err) return err;
	err = plist_pop_num(pl, head, &y);
	if (err) return err;

	plist_push_num(pl, x - y);
	return 0;
}

static error_t fs_exp(plist_t *pl, plist_t *head)
{
	num_t x, y;
	error_t err;

	err = plist_pop_num(pl, head, &x);
	if (err) return err;
	err = plist_pop_num(pl, head, &y);
	if (err) return err;

	/* 0^0 is undefined */
	if (x == 0 && y == 0) {
		return ERR_NOT_VALID;
	}

	plist_push_num(pl, POW(x, y));
	return 0;
}

op_entry ops [] = {
	OP("factor", "computes the prime factors of a number", &fs_factor),
	OP("fib",    "computes the first n fibonacci numbers", &fs_fib),
	OP("add",    "adds two numbers",                       &fs_add),
	OP("sub",    "subtracts two inputs",                   &fs_sub),
	OP("exp",    "raises a number to a given exponent",    &fs_exp),
	OP_END()
};
