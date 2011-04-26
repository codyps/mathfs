#ifndef MATHFS_ERROR_H_
#define MATHFS_ERROR_H_

/* Copyright 2011, Cody Schafer <cpschafer@gmail.com>
 */

typedef enum {
	ERR_NONE = 0,
	ERR_UNDEF_OP,
	ERR_TOO_FEW,
	ERR_NOT_INT,
} error_t;

char const *error_msg(error_t err);
#endif
