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
    if (parser->had_error)
        return;

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
    parser->had_error = false;
    parser->error_msg[0] = '\0';
    parser->tok = lex_next(&parser->lexer);
}

static bool parser_advance(Parser *parser)
{
    parser->tok = lex_next(&parser->lexer);

    if (parser->tok.type == TOKEN_INVALID) {
        parser_errorf(parser,
            "error: invalid token \'%.*s\'\n",
            (int)parser->tok.len,
            parser->tok.base);

        return false;
    }

    return true;
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

        if (!parser_advance(parser))
            return NULL;

        Expr *right = parse_term(parser);

        if (!right) {
            if (parser->tok.type != TOKEN_EOF)
                parser_errorf(parser,
                        "error: expected expression after \'%c\'\n",
                        op);
            else
                parser_errorf(parser,
                        "error: expected expression after \'%c\', got end of input\n",
                        op);
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
            if (!parser_advance(parser))
                return NULL;
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
                        "error: expected expression after \'%c\', got end of input\n",
                        op);
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

        if (!parser_advance(parser))
            return NULL;

        Expr *right = parse_unary(parser);
        if (!right) {
            if (parser->tok.type != TOKEN_EOF)
                parser_errorf(parser,
                        "error: expected expression after \'%c\'\n",
                        op);
            else
                parser_errorf(parser,
                        "error: expected expression afer \'%c\', got end of input\n",
                        op);
            expr_free(left);
            return NULL;
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
        if (!parser_advance(parser))
            return NULL;
        left = expr_unary('!', left);
    }

    return left;
}

Expr *parse_unary(Parser *parser)
{
    if (parser->tok.type == TOKEN_PLUS ||
        parser->tok.type == TOKEN_MINUS) {

        char op = *parser->tok.base;
        if (!parser_advance(parser))
            return NULL;

        Expr *operand = parse_unary(parser);

        if (!operand) {
            parser_errorf(parser,
                    "error: expected expression after '%c'\n",
                    op);
            return NULL;
        }

        return expr_unary(op, operand);
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

            if (!parser_advance(parser))
                return NULL;
            break;

        case TOKEN_IDENT:
            {
                const char *base = parser->tok.base;
                size_t len = parser->tok.len;

                if (!parser_advance(parser))
                    return NULL;

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

                if (!parser_advance(parser))
                    return NULL;

                if (parser->tok.type != TOKEN_RPAREN) {
                    for (;;) {
                        Expr *arg = parse_expr(parser);

                        if (!arg) {
                            parser_errorf(parser,
                                    "error: expected argument expression\n");
                            return NULL;
                        }

                        args[arg_count++] = arg;

                        if (parser->tok.type == TOKEN_COMMA) {
                            if (!parser_advance(parser))
                                return NULL;
                            continue;
                        }

                        break;
                    }
                }

                if (parser->tok.type != TOKEN_RPAREN) {
                    if (parser->tok.type == TOKEN_EOF)
                        parser_errorf(parser,
                                "error: expected ')', got end of input\n");
                    else
                        parser_errorf(parser,
                                "error: expected ')', got '%.*s'\n",
                                (int)parser->tok.len,
                                parser->tok.base);
                    expr_free(left);
                    return NULL;
                }

                if (arg_count == 0) {
                    parser_errorf(parser, "error: function \'%.*s\' : at least a single parameter must be passed\n",
                            (int)len, base);
                    expr_free(left);
                    return NULL;
                }

                if (!parser_advance(parser))
                    return NULL;

                left = expr_call(base, len, args, arg_count);
                
                if (!left->call.args_valid) {
                    parser_errorf(parser,
                            "error: function \'%s\': argument count does not match function prototype\n",
                            left->call.name);
                    expr_free(left);
                    return NULL;
                }

                if (!left) {
                    parser_errorf(parser,
                            "error: expected expression inside parentheses\n");
                    return NULL;
                }

                if (!left->call.func) {
                    parser_errorf(parser, "error: unknown function '%s'\n",
                            left->call.name);
                    expr_free(left);
                    return NULL;
                }

                break;
            }

        case TOKEN_LPAREN:
            if (!parser_advance(parser))
                return NULL;

            left = parse_expr(parser);

            if (!left) {
                if (!parser->had_error) {
                    parser_errorf(parser,
                            "error: expected expression inside parentheses\n");
                }
                return NULL;
            }

            if (parser->tok.type != TOKEN_RPAREN) {
                if (parser->tok.type == TOKEN_EOF)
                    parser_errorf(parser,
                            "error: expected ')', got end of input\n");
                else
                    parser_errorf(parser,
                            "error: expected ')', got '%.*s'\n",
                            (int)parser->tok.len,
                            parser->tok.base);
                expr_free(left);
                return NULL;
            }

            if (!parser_advance(parser))
                return NULL;
            break;

        default:
            return NULL;
    }

    return left;
}
