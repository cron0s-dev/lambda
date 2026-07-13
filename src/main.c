#include "lexer.h"
#include "parser.h"
#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        Expr *expr = parse_expr(&parser);
        if (strcmp(expr->ident, "clear") == 0) {
            printf("\033[2J\033[H");
            continue;
        }

        printf("%g\n", eval_expr(expr));
    }
    free(src);
    
    return 0;
}
