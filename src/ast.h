#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum {
    EXPR_NUM,
    EXPR_IDENT,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL
} ExprType;

typedef struct Expr {
    ExprType type;

    union {
        double num;

        char *ident;

        struct {
            char op;
            struct Expr *left;
            struct Expr *right;
        } binary;

        struct {
            char op;
            struct Expr *operand;
        } unary;

        struct {
            char* name;
            struct Expr **args;
            size_t arg_count;
        } call;
    };
} Expr;

Expr *expr_num(double value);
Expr *expr_ident(const char *base, size_t len);
Expr *expr_binary(char op, Expr *left, Expr *right);
Expr *expr_unary(char op, Expr *operand);
Expr *expr_call(const char *base, size_t len, Expr **args, size_t arg_count);

#endif
