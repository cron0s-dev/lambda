#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer lexer;
    Token tok;
} Parser;

void parser_init(Parser *p, Lexer *lexer);
Expr *parse_expr(Parser *parser);
Expr *parse_term(Parser *parser);
Expr *parse_power(Parser *parser);
Expr *parse_postfix(Parser *parser);
Expr *parse_unary(Parser *parser);
Expr *parse_primary(Parser *parser);

#endif
