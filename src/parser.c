#include "parser.h"
#include "lexer.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

void parser_errorf(Parser *parser, const char *fmt, ...)
{
    va_list args;

    parser->had_error = true;

    va_start(args, fmt);
    vsnprintf(parser->error_msg,
              sizeof(parser->error_msg),
              fmt,
              args);
    va_end(args);
}

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

    if (!expr) {
        return NULL;
    }

    while (parser->tok.type == TOKEN_PLUS ||
           parser->tok.type == TOKEN_MINUS) {
        char op = *parser->tok.base;

        parser_advance(parser);

        Expr *right = parse_term(parser);

        if (!right) {
            return NULL;
        }

        expr = expr_binary(op, expr, right);

        if (!expr) {
            return NULL;
        }
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

    if (!expr) {
        return NULL;
    }

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

        Expr *right = parse_power(parser);

        if (!right) {
            return NULL;
        }

        expr = expr_binary(op, expr, right);

        if (!expr) {
            return NULL;
        }
    }

    return expr;
}

Expr *parse_power(Parser *parser)
{
    Expr *expr = parse_unary(parser);

    if (!expr) {
        return NULL;
    }

    if (parser->tok.type == TOKEN_CARET) {
        char op = *parser->tok.base;

        parser_advance(parser);

        Expr *right = parse_power(parser);

        if (!right) {
            return NULL;
        }

        expr = expr_binary(op, expr, right);

        if (!expr) {
            return NULL;
        }
    }

    return expr;
}

Expr *parse_postfix(Parser *parser)
{
    Expr *expr = parse_primary(parser);

    if (!expr) {
        return NULL;
    }

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

            if (!expr) {
                return NULL;
            }

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
                        Expr *arg = parse_expr(parser);

                        if (!arg) {
                            return NULL;
                        }

                        args[arg_count++] = arg;

                        if (parser->tok.type == TOKEN_COMMA) {
                            parser_advance(parser);
                            continue;
                        }

                        break;
                    }
                }

                if (parser->tok.type != TOKEN_RPAREN) {
                    if (parser->tok.type == TOKEN_EOF)
                        parser_errorf(parser, "expected ')', got end of input\n");
                    else
                        parser_errorf(parser,
                                "expected ')', got '%.*s'\n",
                                (int)parser->tok.len,
                                parser->tok.base);
                    expr_free(expr);
                    return NULL;
                }

                if (arg_count == 0) {
                    parser_errorf(parser, "error: %.*s: at least a single parameter must be passed\n",
                            (int)len, base);
                    expr_free(expr);
                    return NULL;
                }

                parser_advance(parser);

                expr = expr_call(base, len, args, arg_count);

                if (!expr->call.func) {
                    parser_errorf(parser, "error: unknown function '%s'\n",
                            expr->call.name);
                    expr_free(expr);
                    return NULL;
                }

                break;
            }

        case TOKEN_LPAREN:
            parser_advance(parser);

            expr = parse_expr(parser);

            if (!expr)
                return NULL;

            if (parser->tok.type != TOKEN_RPAREN) {
                parser_errorf(parser, "error: expected \')\', got \'%c\'\n",
                        *parser->tok.base);
                expr_free(expr);
                return NULL;
            }

            parser_advance(parser);
            break;

        default:
            parser_errorf(parser, "error: invalid expression\n");
            expr_free(expr);
            return NULL;
    }

    return expr;
}
