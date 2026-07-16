#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "color.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT_SYM "λ"
#define PROMPT_CHAR '>'
#define PROMPT "%s%s%s %c ",\
            COLOR_MAGENTA,\
            PROMPT_SYM,\
            COLOR_RESET,\
            PROMPT_CHAR

#ifdef _WIN32

#include <windows.h>

char *get_input(void)
{
    char *src = malloc(512);
    fgets(src, 512, stdin);

    src[strcspn(src, "\n")] = '\0';
    return src;
}

#else

#include <readline/readline.h>
#include <readline/history.h>

char *get_input(void)
{
    char prompt[64];
    snprintf(prompt, 64, PROMPT);
    char *line = readline(prompt);

    if (line && *line)
        add_history(line);

    return line;
}

#endif

void print_error(const char *msg)
{
    putc('\n', stdout);
    fprintf(stderr, " %serror:%s %s", COLOR_RED, COLOR_RESET, msg);
    putc('\n', stdout);
}

void print_expr_result(const char* line, const Expr *expr)
{
    putc('\n', stdout);
    printf(" %s%s%s = %s%.15g%s\n",
            COLOR_BOLD,
            COLOR_RESET,
            line,
            COLOR_GREEN,
            eval_expr(expr),
            COLOR_RESET);
    putc('\n', stdout);
}

void term_clear()
{
    printf(TERM_CLEAR);
}

int main(void)
{
    for (;;) {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        printf(PROMPT);
#endif
        char *line = get_input();

        Lexer lexer;
        lex_init(&lexer, line);

        Parser parser;
        parser_init(&parser, &lexer);

        Expr *expr = parse_expr(&parser);
        if (!expr) {
            print_error(parser.error_msg);
            continue;
        }

        if (expr->type == EXPR_IDENT && 
            strcmp(expr->ident, "var") == 0) {

            for (size_t i = 0; i < sizeof(constants) / sizeof(constants[0]); i++)
                printf("%s = %.17g\n", constants[i].name, constants[i].value);
            continue;
        }

        if (expr->type == EXPR_IDENT && 
            strcmp(expr->ident, "clear") == 0) {
            term_clear();
            continue;
        }

        if (expr->type == EXPR_IDENT && 
            strcmp(expr->ident, "exit") == 0) {
            exit(0);
        }

        print_expr_result(line, expr);
        expr_free(expr);

        free(line);
    }
    
    return 0;
}
