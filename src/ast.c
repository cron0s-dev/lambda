#include "ast.h"
#include "func.h"
#include "hash_map.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern HashMap *hm_func;

static char *slicetstr(const char *base, size_t len) {
    char *str = malloc(len + 1);
    if (!str)
        return NULL;

    memcpy(str, base, len);
    str[len] = '\0';

    return str;
}

Expr *expr_num(const char *base, size_t len) {
    char *str = slicetstr(base, len);
    if (!str)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type = EXPR_NUM;
    expr->num  = strtod(str, NULL);

    free(str);
    return expr;
}

Expr *expr_ident(const char *base, size_t len) {
    if (!base || len <= 0)
        return NULL;

    char *str = slicetstr(base, len);
    if (!str)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type  = EXPR_IDENT;
    expr->ident = str;

    return expr;
}

Expr *expr_binary(char op, Expr *left, Expr *right) {
    if (!left || !right)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type         = EXPR_BINARY;
    expr->binary.op    = op;
    expr->binary.left  = left;
    expr->binary.right = right;

    return expr;
}

Expr *expr_unary(char op, Expr *operand) {
    if (!operand)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type          = EXPR_UNARY;
    expr->unary.op      = op;
    expr->unary.operand = operand;

    return expr;
}

Expr *expr_call(const char *base, size_t len, Expr **args, size_t arg_count) {
    if (!base || len <= 0 || !args || arg_count <= 0)
        return NULL;

    char *str = slicetstr(base, len);
    if (!str)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type            = EXPR_CALL;
    expr->call.name       = str;
    expr->call.func       = NULL;
    expr->call.args       = args;
    expr->call.arg_count  = arg_count;
    expr->call.args_valid = true;

    Builtin *builtin = hm_get(hm_func, expr->call.name);
    if (!builtin)
        return expr;

    if (arg_count != builtin->arg_count && builtin->arg_count >= 0)
        expr->call.args_valid = false;

    expr->call.func = builtin->func;

    return expr;
}

void expr_free(Expr *expr) {
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

        case EXPR_ASSIGN:
            free(expr->assign.name);
            free(expr->assign.value);
            break;

        default:
            break;
    }

    free(expr);
}

Expr *expr_assign(const char *base, size_t len, Expr *value) {
    if (!base || !value || len <= 0)
        return NULL;

    Expr *expr = malloc(sizeof(*expr));
    if (!expr)
        return NULL;

    expr->type         = EXPR_ASSIGN;
    expr->assign.name  = slicetstr(base, len);
    expr->assign.value = value;

    return expr;
}
