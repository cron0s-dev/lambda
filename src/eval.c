#include "eval.h"
#include "ast.h"
#include "hash_map.h"

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

extern HashMap *hm_const;
extern HashMap *hm_var;

double eval_expr(const Expr *expr)
{
    double res = 0.0;
    switch (expr->type) {
        case EXPR_NUM:
            res = expr->num;
            break;

        case EXPR_UNARY:
            res = eval_unary(expr);
            break;

        case EXPR_BINARY:
            res = eval_binary(expr);
            break;

        case EXPR_CALL:
            res = eval_call(expr);
            break;

        case EXPR_IDENT:
            res = eval_ident(expr);
            break;

        case EXPR_ASSIGN:
            res = eval_expr(expr->assign.value);
            if (res == 0)
                hm_rm(hm_var, expr->assign.name);

            hm_ins(hm_var, expr->assign.name, expr->assign.value);
            break;
    }

    return res;
}

double eval_binary(const Expr *expr)
{
    double left = eval_expr(expr->binary.left);
    double right = eval_expr(expr->binary.right);

    switch (expr->binary.op) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            if (right == 0)
                return NAN;
            return left / right;
        case '%':
            return fmod(left, right);
        case '^':
            return pow(left, right);
    }

    return 0.0;
}

double eval_call(const Expr *expr)
{
    double val = 0.0;

    double args[expr->call.arg_count];

    for (size_t j = 0; j < expr->call.arg_count; j++)
        args[j] = eval_expr(expr->call.args[j]);

    val = expr->call.func(args, expr->call.arg_count);

    val = fabs(val) < 1e-15 ? 0 : val;

    return val;
}

double eval_unary(const Expr *expr)
{
    double operand = eval_expr(expr->unary.operand);

    switch (expr->unary.op) {
        case '+':
            return operand;

        case '-':
            return -operand;

        case '!':
            return tgamma(operand + 1.0);

        default:
            return operand; 
    }

    return 0.0;
}

double eval_ident(const Expr *expr)
{
    double *const_var = hm_get(hm_const, expr->ident);
    if (const_var)
        return *const_var;

    Expr *var = hm_get(hm_var, expr->ident);
    if (var)
        return eval_expr(var);

    return 0.0;
}
