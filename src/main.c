#include "ast.h"
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
        printf("λ > ");

        fgets(src, 512, stdin);

        src[strcspn(src, "\n")] = '\0';

        Lexer lexer;
        lex_init(&lexer, src);

        Parser parser;
        parser_init(&parser, &lexer);

        Expr *expr = parse_expr(&parser);
        if (!expr) {
           fprintf(stderr, " %s", parser.error_msg);
           continue;
        }

        if (expr->type == EXPR_IDENT && 
            strcmp(expr->ident, "clear") == 0) {
            printf("\033[2J\033[H");
            continue;
        }

        if (expr->type == EXPR_IDENT && 
            strcmp(expr->ident, "exit") == 0) {
            exit(0);
        }

        printf(" %s = %.17g\n", src, eval_expr(expr));
        expr_free(expr);
    }
    free(src);
    
    return 0;
}
