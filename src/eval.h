#ifndef EVAL_H
#define EVAL_H

#include "parser.h"

double eval_expr(const Expr *expr);
double eval_unary(const Expr *expr);
double eval_binary(const Expr *expr);
double eval_call(const Expr *expr);
double eval_ident(const Expr *expr);

#endif
