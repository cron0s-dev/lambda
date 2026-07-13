#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

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

int main(void)
{
    for (;;) {
        printf(">> ");

        char *src = malloc(512);
        fgets(src, 512, stdin);

        src[strcspn(src, "\n")] = '\0';

        Lexer lexer;
        lexer_init(&lexer, src);

        for (;;) {
            Token tok = lexer_next(&lexer);

            if (tok.type == TOKEN_EOF)
                break;

            if (tok.type == TOKEN_INVALID) {
                fprintf(stderr, "error: invalid token, \'%.*s\'\n",
                        (int)tok.len, tok.base);
                break;
            }

            print_token(tok);
        }
    }

    return 0;
}
