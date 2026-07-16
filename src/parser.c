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
    char op = '\0';
    if (parser->tok.type != TOKEN_EOF)
        op = *parser->tok.base;
    
    Expr *left = parse_term(parser);

    if (!left) {
        if (parser->tok.type != TOKEN_EOF)
            parser_errorf(parser,
                    "error: expected expression before \'%c\'\n",
                    op);
        return NULL;
    }

    while (parser->tok.type == TOKEN_PLUS ||
           parser->tok.type == TOKEN_MINUS) {
        op = *parser->tok.base;

        parser_advance(parser);

        Expr *right = parse_term(parser);

        if (!right) {
            if (parser->tok.type != TOKEN_EOF)
                parser_errorf(parser,
                        "error: expected expression after \'%c\'\n",
                        op);
            else
                parser_errorf(parser,
                        "error: expected expression, got end of input\n");
            return NULL;
        }

        left = expr_binary(op, left, right);

        if (!left) {
            return NULL;
        }
    }

    return left;
}

bool starts_primary(TokenType t)
{
    return t == TOKEN_NUM   ||
           t == TOKEN_IDENT ||
           t == TOKEN_LPAREN;
}

Expr *parse_term(Parser *parser)
{
    char op = '\0';
    if (parser->tok.type != TOKEN_EOF)
        op = *parser->tok.base;

    Expr *left = parse_unary(parser);

    if (!left) {
        if (parser->tok.type != TOKEN_EOF)
            parser_errorf(parser,
                    "error: expected expression before \'%c\'\n",
                    op);
        return NULL;
    }

    while (parser->tok.type == TOKEN_STAR    ||
           parser->tok.type == TOKEN_SLASH   ||
           parser->tok.type == TOKEN_PERCENT ||
           starts_primary(parser->tok.type)) {

        if (parser->tok.type == TOKEN_STAR ||
            parser->tok.type == TOKEN_SLASH ||
            parser->tok.type == TOKEN_PERCENT) {
            op = *parser->tok.base;
            parser_advance(parser);
        } else 
            op = '*';

        Expr *right = parse_unary(parser);

        if (!right) {
            if (parser->tok.type != TOKEN_EOF)
                parser_errorf(parser,
                        "error: expected expression after \'%c\'\n",
                        op);
            else
                parser_errorf(parser,
                        "error: expected expression, got end of input\n");
            return NULL;
        }

        left = expr_binary(op, left, right);

        if (!left) {
            return NULL;
        }
    }

    return left;
}

Expr *parse_power(Parser *parser)
{
    char op = '\0';
    if (parser->tok.type != TOKEN_EOF)
        op = *parser->tok.base;

    Expr *left = parse_postfix(parser);

    if (!left) {
        if (parser->tok.type != TOKEN_EOF)
            parser_errorf(parser,
                    "error: expected expression before \'%c\'\n",
                    op);
        return NULL;
    }

    if (parser->tok.type == TOKEN_CARET) {
        op = *parser->tok.base;

        parser_advance(parser);

        Expr *right = parse_unary(parser);
        if (!right) {
            if (parser->tok.type != TOKEN_EOF)
                parser_errorf(parser,
                        "error: expected expression after \'%c\'\n",
                        op);
            else
                parser_errorf(parser,
                        "error: expected expression, got end of input\n");
        }

        left = expr_binary(op, left, right);

        if (!left) {
            return NULL;
        }
    }

    return left;
}

Expr *parse_postfix(Parser *parser)
{
    Expr *left = parse_primary(parser);

    if (!left) {
        return NULL;
    }

    while (parser->tok.type == TOKEN_EXCLAMATION)
    {
        parser_advance(parser);
        left = expr_unary('!', left);
    }

    return left;
}

Expr *parse_unary(Parser *parser)
{
    if (parser->tok.type == TOKEN_PLUS ||
        parser->tok.type == TOKEN_MINUS) {

        char op = *parser->tok.base;
        parser_advance(parser);

        return expr_unary(op, parse_unary(parser));
    }

    return parse_power(parser);
}

Expr *parse_primary(Parser *parser)
{
    Expr *left = NULL;

    switch (parser->tok.type) {
        case TOKEN_NUM:
            left = expr_num(parser->tok.base, parser->tok.len);

            if (!left) {
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
                    left = expr_ident(base, len);
                    break;
                }

                Expr **args = malloc(sizeof(*args) * 16);
                if (!args) {
                    free(args);
                    return NULL;
                }

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
                        parser_errorf(parser, "error: expected ')', got end of input\n");
                    else
                        parser_errorf(parser,
                                "error: expected ')', got '%.*s'\n",
                                (int)parser->tok.len,
                                parser->tok.base);
                    expr_free(left);
                    return NULL;
                }

                if (arg_count == 0) {
                    parser_errorf(parser, "error: %.*s: at least a single parameter must be passed\n",
                            (int)len, base);
                    expr_free(left);
                    return NULL;
                }

                parser_advance(parser);

                left = expr_call(base, len, args, arg_count);

                if (!left)
                    return NULL;

                if (!left->call.func) {
                    parser_errorf(parser, "error: unknown function '%s'\n",
                            left->call.name);
                    expr_free(left);
                    return NULL;
                }

                break;
            }

        case TOKEN_LPAREN:
            parser_advance(parser);

            left = parse_expr(parser);

            if (!left)
                return NULL;

            if (parser->tok.type != TOKEN_RPAREN) {
                if (parser->tok.type != TOKEN_EOF)
                    parser_errorf(parser, "error: expected \')\', got \'%c\'\n",
                            *parser->tok.base);
                expr_free(left);
                return NULL;
            }

            parser_advance(parser);
            break;

        default:
            parser_errorf(parser, "error: invalid expression\n");
            return NULL;
    }

    return left;
}
