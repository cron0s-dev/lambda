#include "eval.h"
#include "ast.h"
#include "func.h"

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

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
    for (size_t i = 0; i < sizeof(constants) / sizeof(constants[0]); i++) {
        if (strcmp(expr->ident, constants[i].name) == 0)
            return constants[i].value;
    }

    return 0.0;
}
