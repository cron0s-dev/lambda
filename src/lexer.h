#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_NUM,
    TOKEN_IDENT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_CARET,
    TOKEN_EXCLAMATION,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EQUAL,
    TOKEN_COMMA,
    TOKEN_INVALID
} TokenType;

typedef struct {
    const char *base;
    size_t len;
    TokenType type;
} Token;

typedef struct {
    const char *src;
    const char *start;
    const char *p;
} Lexer;

void lex_init(Lexer *lexer, const char *src);
Token lex_next(Lexer *lexer);

#endif
