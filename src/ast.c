#include "ast.h"

#include <string.h>
#include <stdlib.h>

static char *slicetstr(const char *base, size_t len)
{
    char *str = malloc(len + 1);
    if (!str)
        return NULL;

    memcpy(str, base, len);
    str[len] = '\0';

    return str;
}

Expr *expr_num(const char *base, size_t len)
{
    char *str = slicetstr(base, len);
    if (!str)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type = EXPR_NUM;
    expr->num = strtod(str, NULL);

    return expr;
}

Expr *expr_ident(const char *base, size_t len)
{
    if (!base || len <= 0)
        return NULL;

    char *str = slicetstr(base, len);
    if (!str)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type = EXPR_IDENT;
    expr->ident = str;

    return expr;
}

Expr *expr_binary(char op, Expr *left, Expr *right)
{
    if (!left || !right)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type = EXPR_BINARY;
    expr->binary.op = op;
    expr->binary.left = left;
    expr->binary.right = right;

    return expr;
}

Expr *expr_unary(char op, Expr *operand)
{
    if (!operand)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type = EXPR_UNARY;
    expr->unary.op = op;
    expr->unary.operand = operand;

    return expr;
}

Expr *expr_call(const char *base, size_t len, Expr **args, size_t arg_count)
{
    if (!base    ||
        len <= 0 ||
        !args    ||
        arg_count <= 0)
        return NULL;

    char *str = slicetstr(base, len);
    if (!str)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type = EXPR_CALL;
    expr->call.name = str;
    expr->call.args = args;
    expr->call.arg_count = arg_count;

    return expr;
}

void expr_free(Expr *expr)
{
    if (!expr)
        return;

    switch (expr->type) {
        case EXPR_IDENT:
            free(expr->ident);
            break;

        case EXPR_BINARY:
            expr_free(expr->binary.left);
            expr_free(expr->binary.right);
            break;

        case EXPR_UNARY:
            expr_free(expr->unary.operand);
            break;

        case EXPR_CALL:
            free(expr->call.name);
            for (size_t i = 0; i < expr->call.arg_count; i++)
                expr_free(expr->call.args[i]);
            free(expr->call.args);
            break;
        
        default:
            break;
    }

    free(expr);
}
