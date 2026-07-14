#include "parser.h"
#include "lexer.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void parser_init(Parser *parser, Lexer *lexer)
{
    parser->lexer = *lexer;    
    parser->tok = lex_next(&parser->lexer);
}

static void parser_advance(Parser *parser)
{
    parser->tok = lex_next(&parser->lexer);
}

Expr *parse_expr(Parser *parser)
{
    Expr *expr = parse_term(parser);

    while (parser->tok.type == TOKEN_PLUS ||
           parser->tok.type == TOKEN_MINUS) {
        char op = *parser->tok.base;

        parser_advance(parser);

        expr = expr_binary(op, expr, parse_term(parser));
    }

    return expr;
}

bool starts_primary(TokenType t)
{
    return t == TOKEN_NUM   ||
           t == TOKEN_IDENT ||
           t == TOKEN_LPAREN;
}

Expr *parse_term(Parser *parser)
{
    Expr *expr = parse_power(parser);

    while (parser->tok.type == TOKEN_STAR    ||
           parser->tok.type == TOKEN_SLASH   ||
           parser->tok.type == TOKEN_PERCENT ||
           starts_primary(parser->tok.type)) {
        char op;

        if (parser->tok.type == TOKEN_STAR ||
            parser->tok.type == TOKEN_SLASH ||
            parser->tok.type == TOKEN_PERCENT) {
            op = *parser->tok.base;
            parser_advance(parser);
        } else 
            op = '*';

        expr = expr_binary(op, expr, parse_power(parser));
    }

    return expr;
}

Expr *parse_power(Parser *parser)
{
    Expr *expr = parse_unary(parser);

    if (parser->tok.type == TOKEN_CARET) {
        char op = *parser->tok.base;

        parser_advance(parser);

        expr = expr_binary(op, expr, parse_power(parser));
    }

    return expr;
}

Expr *parse_postfix(Parser *parser)
{
    Expr *expr = parse_primary(parser);

    while (parser->tok.type == TOKEN_EXCLAMATION)
    {
        parser_advance(parser);
        expr = expr_unary('!', expr);
    }

    return expr;
}

Expr *parse_unary(Parser *parser)
{
    if (parser->tok.type == TOKEN_PLUS ||
        parser->tok.type == TOKEN_MINUS)
    {
        char op = *parser->tok.base;
        parser_advance(parser);
        return expr_unary(op, parse_unary(parser));
    }

    return parse_postfix(parser);
}

Expr *parse_primary(Parser *parser)
{
    Expr *expr = NULL;

    switch (parser->tok.type) {
        case TOKEN_NUM:
            expr = expr_num(parser->tok.base, parser->tok.len);
            parser_advance(parser);
            break;

        case TOKEN_IDENT:
            {
                const char *base = parser->tok.base;
                size_t len = parser->tok.len;

                parser_advance(parser);

                if (parser->tok.type != TOKEN_LPAREN) {
                    expr = expr_ident(base, len);
                    break;
                }

                Expr **args = malloc(sizeof(*args) * 16);
                if (!args)
                    return NULL;

                size_t arg_count = 0;

                parser_advance(parser);

                if (parser->tok.type != TOKEN_RPAREN) {
                    for (;;) {
                        args[arg_count++] = parse_expr(parser);

                        if (parser->tok.type == TOKEN_COMMA) {
                            parser_advance(parser);
                            continue;
                        }

                        break;
                    }
                }

                if (parser->tok.type != TOKEN_RPAREN) {
                    fprintf(stderr, "error: expected ')', got '%c'\n",
                            *parser->tok.base);
                    exit(1);
                }

                parser_advance(parser);

                expr = expr_call(base, len, args, arg_count);
                break;
            }

        case TOKEN_LPAREN:
            parser_advance(parser);

            expr = parse_expr(parser);

            if (parser->tok.type != TOKEN_RPAREN) {
                fprintf(stderr, "error: expected \')\', got \'%c\'\n",
                        *parser->tok.base);
                exit(1);
            }

            parser_advance(parser);
            break;

        default:
            fprintf(stderr, "error: invalid expression\n");
            exit(1);
            break;
    }

    return expr;
}
