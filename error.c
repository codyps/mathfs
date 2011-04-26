/* Copyright 2011, Cody Schafer <cpschafer@gmail.com>
 */

#include "error.h"

char const *error_msg(error_t err)
{
	switch (err) {
	case ERR_NONE:
		return "";
	case ERR_UNDEF_OP:
		return "undefined operation";
	case ERR_TOO_FEW:
		return "too few parameters";
	case ERR_NOT_INT:
		return "parameter must be an integer";
	default:
		return "an unknown error has occured";
	}
}
