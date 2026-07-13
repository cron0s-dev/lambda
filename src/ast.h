#ifndef AST_H
#define AST_H

typedef enum {
    EXPR_NUM,
    EXPR_BINARY,
    EXPR_UNARY
} ExprType;

typedef struct Expr {
    ExprType type;

    union {
        double num;

        struct {
            char op;
            Expr *left;
            Expr *right;
        } binary;

        struct {
            char op;
            Expr *operand;
        } unary;
    };
} Expr;

#endif
