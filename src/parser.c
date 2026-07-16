#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "color.h"

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
            "invalid token \'%s%.*s%s\'\n",
            COLOR_YELLOW,
            (int)parser->tok.len,
            COLOR_RESET,
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
                    "expected expression before \'%s%c%s\'\n",
                    COLOR_YELLOW,
                    op,
                    COLOR_RESET);
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
                        "expected expression after \'%s%c%s\'\n",
                        COLOR_YELLOW,
                        op,
                        COLOR_RESET);
            else
                parser_errorf(parser,
                        "expected expression after \'%s%c%s\', got end of input\n",
                        COLOR_YELLOW,
                        op,
                        COLOR_RESET);
            return NULL;
        }

        left = expr_binary(op, left, right);

        if (!left) {
            return NULL;
        }
    }

    return left;
}

bool starts_primary(TokenKind t)
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
                    "expected expression before \'%s%c%s\'\n",
                    COLOR_YELLOW,
                    op,
                    COLOR_RESET);
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
                        "expected expression after \'%s%c%s\'\n",
                        COLOR_YELLOW,
                        op,
                        COLOR_RESET);
            else
                parser_errorf(parser,
                        "expected expression after \'%s%c%s\', got end of input\n",
                        COLOR_YELLOW,
                        op,
                        COLOR_RESET);
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
                    "expected expression before \'%s%c%s\'\n",
                    COLOR_YELLOW,
                    op,
                    COLOR_RESET);
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
                        "expected expression after \'%s%c%s\'\n",
                        COLOR_YELLOW,
                        op,
                        COLOR_RESET);
            else
                parser_errorf(parser,
                        "expected expression after \'%s%c%s\', got end of input\n",
                        COLOR_YELLOW,
                        op,
                        COLOR_RESET);
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
                    "expected expression after \'%s%c%s\'\n",
                    COLOR_YELLOW,
                    op,
                    COLOR_RESET);
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
                                    "function \'%s%.*s%s\': expected argument expression\n",
                                    COLOR_CYAN,
                                    (int)len,
                                    base,
                                    COLOR_RESET);
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
                                "expected '%s)%s', got end of input\n",
                                COLOR_YELLOW,
                                COLOR_RESET);
                    else
                        parser_errorf(parser,
                                "expected '%s)%s', got '%s%.*s%s'\n",
                                COLOR_YELLOW,
                                COLOR_RESET,
                                COLOR_YELLOW,
                                (int)parser->tok.len,
                                parser->tok.base,
                                COLOR_RESET);
                    expr_free(left);
                    return NULL;
                }

                if (arg_count == 0) {
                    parser_errorf(parser,
                            "function \'%s%.*s%s\' : at least a single parameter must be passed\n",
                            COLOR_CYAN,
                            (int)len,
                            base,
                            COLOR_RESET);
                    expr_free(left);
                    return NULL;
                }

                if (!parser_advance(parser))
                    return NULL;

                left = expr_call(base, len, args, arg_count);
                
                if (!left->call.args_valid) {
                    parser_errorf(parser,
                            "function \'%s%.*s%s\': argument count does not match function prototype\n",
                            COLOR_CYAN,
                            (int)len,
                            base,
                            COLOR_RESET);
                    expr_free(left);
                    return NULL;
                }

                if (!left) {
                    parser_errorf(parser,
                            "expected expression inside parentheses\n");
                    return NULL;
                }

                if (!left->call.func) {
                    parser_errorf(parser, "unknown function '%s%.*s%s'\n",
                            COLOR_YELLOW,
                            (int)len,
                            base,
                            COLOR_RESET);
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
                            "expected expression inside parentheses\n");
                }
                return NULL;
            }

            if (parser->tok.type != TOKEN_RPAREN) {
                if (parser->tok.type == TOKEN_EOF)
                    parser_errorf(parser,
                            "expected '%s)%s', got end of input\n",
                            COLOR_YELLOW,
                            COLOR_RESET);
                else
                    parser_errorf(parser,
                            "expected '%s)%s', got '%.*s'\n",
                            COLOR_YELLOW,
                            COLOR_RESET,
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
