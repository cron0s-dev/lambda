#include "eval.h"
#include "ast.h"

#include <math.h>
#include <string.h>

double eval_expr(const Expr *expr)
{
    switch (expr->type) {
        case EXPR_NUM:
            return expr->num;

        case EXPR_UNARY:
            return eval_unary(expr);

        case EXPR_BINARY:
            return eval_binary(expr);

        case EXPR_CALL:
            return eval_call(expr);

        case EXPR_IDENT:
            return eval_ident(expr);
            break;
    }

    return 0.0;
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
    double arg = eval_expr(expr->call.args[0]);

    if (strcmp(expr->call.name, "sin") == 0)
        return sin(arg);

    if (strcmp(expr->call.name, "cos") == 0)
        return cos(arg);

    if (strcmp(expr->call.name, "tan") == 0)
        return tan(arg);

    if (strcmp(expr->call.name, "asin") == 0)
        return asin(arg);

    if (strcmp(expr->call.name, "acos") == 0)
        return acos(arg);

    if (strcmp(expr->call.name, "atan") == 0)
        return atan(arg);

    if (strcmp(expr->call.name, "csc") == 0)
        return 1.0 / sin(arg);

    if (strcmp(expr->call.name, "sec") == 0)
        return 1.0 / cos(arg);

    if (strcmp(expr->call.name, "cot") == 0)
        return 1.0 / tan(arg);

    if (strcmp(expr->call.name, "log") == 0) {
        if (expr->call.arg_count == 1) 
            return log10(arg);

        double x = eval_expr(expr->call.args[0]);
        double y = eval_expr(expr->call.args[1]);

        return log(x) / log(y);
    }
    if (strcmp(expr->call.name, "log2") == 0)
        return log2(arg);

    if (strcmp(expr->call.name, "ln") == 0)
        return log(arg);

    return 0.0;
}

double eval_unary(const Expr *expr)
{
    double operand = eval_expr(expr->unary.operand);

    switch (expr->unary.op) {
        case '+':
            return operand;

        case '-':
            return -operand;
    }

    return 0.0;
}

double eval_ident(const Expr *expr)
{
    if (strcmp(expr->ident, "pi") == 0)
        return M_PI;

    if (strcmp(expr->ident, "e") == 0)
        return M_E;

    return 0.0;
}
