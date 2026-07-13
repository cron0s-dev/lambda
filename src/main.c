#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

void print_token(Token tok)
{
    const char *type;

    switch (tok.type)
    {
        case TOKEN_NUM:
            type = "NUM";
            break;

        case TOKEN_IDENT:
            type = "IDENTIFIER";
            break;

        case TOKEN_PLUS:
            type = "PLUS";
            break;

        case TOKEN_MINUS:
            type = "MINUS";
            break;

        case TOKEN_STAR:
            type = "STAR";
            break;

        case TOKEN_SLASH:
            type = "SLASH";
            break;

        case TOKEN_PERCENT:
            type = "PERCENT";
            break;

        case TOKEN_CARET:
            type = "STAR";
            break;

        case TOKEN_LPAREN:
            type = "LPAREN";
            break;

        case TOKEN_RPAREN:
            type = "RPAREN";
            break;

        case TOKEN_COMMA:
            type = "COMMA";
            break;

        case TOKEN_EOF:
            type = "EOF";
            break;

        default:
            type = "INVALID";
            break;
    }

    printf("%-12s '%.*s'\n",
           type,
           (int)tok.len,
           tok.base);
}

static void print_indent(int depth)
{
    for (int i = 0; i < depth; i++)
        printf("    ");
}

void expr_print_tree(const Expr *expr, int depth)
{
    if (!expr)
        return;

    print_indent(depth);

    switch (expr->type) {
    case EXPR_NUM:
        printf("NUM: %f\n", expr->num);
        break;

    case EXPR_IDENT:
        printf("IDENT: %s\n", expr->ident);
        break;

    case EXPR_UNARY:
        printf("UNARY: %c\n", expr->unary.op);
        expr_print_tree(expr->unary.operand, depth + 1);
        break;

    case EXPR_BINARY:
        printf("BINARY: %c\n", expr->binary.op);

        print_indent(depth + 1);
        printf("left:\n");
        expr_print_tree(expr->binary.left, depth + 2);

        print_indent(depth + 1);
        printf("right:\n");
        expr_print_tree(expr->binary.right, depth + 2);
        break;

    case EXPR_CALL:
        printf("CALL: %s\n", expr->call.name);

        for (size_t i = 0; i < expr->call.arg_count; i++) {
            print_indent(depth + 1);
            printf("arg %zu:\n", i);
            expr_print_tree(expr->call.args[i], depth + 2);
        }
        break;

    default:
        printf("UNKNOWN\n");
        break;
    }
}

int main(void)
{
    char *src = malloc(512);
    for (;;) {
        printf(">> ");

        fgets(src, 512, stdin);

        src[strcspn(src, "\n")] = '\0';

        Lexer lexer;
        lex_init(&lexer, src);

        Parser parser;
        parser_init(&parser, &lexer);
        Expr *root = parse_expr(&parser);
        expr_print_tree(root, 0);
    }
    free(src);
    
    return 0;
}
