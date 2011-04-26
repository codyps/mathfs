#ifndef OP_H_
#define OP_H_

#include "error.h"
#include "parse.h"

#define OP(name, doc, func) { name, doc, func }
#define OP_END() {}

const extern op_entry ops[];

#endif
