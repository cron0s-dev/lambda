#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>

static void skip_ws(Lexer *lexer);
static Token lex_num(Lexer *lexer);
static Token lex_ident(Lexer *lexer);
static Token lex_op(Lexer *lexer);

void lexer_init(Lexer *lexer, const char *src)
{
    lexer->src = src;
    lexer->start = src;
    lexer->p = src;
}

Token lexer_next(Lexer *lexer)
{
    skip_ws(lexer);

    if (isdigit((unsigned char)*lexer->start))
        return lex_num(lexer);

    if (isalpha((unsigned char)*lexer->start) || *lexer->p == '_')
        return lex_ident(lexer);

    return lex_op(lexer);
}

static Token lex_num(Lexer *lexer)
{
    Token tok = {0};

    tok.base = lexer->start;

    bool dot = false;

    /* checks to see if character is a number
    or a dot. If we have already seen a dot,
    then we stop. Otherwise, we continue. */
    while (isdigit((unsigned char)*lexer->p) ||
           (!dot && (dot = *lexer->p == '.'))) {
        lexer->p++;
    }

    tok.len = lexer->p - lexer->start;
    tok.type = TOKEN_NUM;

    return tok;
}

static Token lex_ident(Lexer *lexer)
{
    Token tok = {0};

    tok.base = lexer->start;

    while (isalpha((unsigned char)*lexer->p) || *lexer->p == '_')
        lexer->p++;

    tok.len = lexer->p - lexer->start;
    tok.type = TOKEN_IDENT;

    return tok;
}

static Token lex_op(Lexer *lexer)
{
    Token tok = {0};

    tok.base = lexer->start;
    tok.len = 1; 

    char c = *lexer->p++;

    switch (c) {
        case '\0':
            tok.type = TOKEN_EOF;
            tok.len = 0;
            break;
        case '+':
            tok.type = TOKEN_PLUS;
            break;
        case '-':
            tok.type = TOKEN_MINUS;
            break;
        case '*':
            tok.type = TOKEN_STAR;
            break;
        case '/':
            tok.type = TOKEN_SLASH;
            break;
        case '%':
            tok.type = TOKEN_PERCENT;
            break;
        case '^':
            tok.type = TOKEN_CARET;
            break;
        case '(':
            tok.type = TOKEN_LPAREN;
            break;
        case ')':
            tok.type = TOKEN_RPAREN;
            break;
        case '=':
            tok.type = TOKEN_EQUAL;
            break;
        case ',':
            tok.type = TOKEN_COMMA;
            break;
        default:
            tok.type = TOKEN_INVALID;
            break;
    }

    return tok;
}

static void skip_ws(Lexer *lexer)
{
    while (isspace((unsigned char)*lexer->p))
        lexer->p++;
    lexer->start = lexer->p;
}
