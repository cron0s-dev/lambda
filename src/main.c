#include "ast.h"
#include "func.h"
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "color.h"
#include "hash_map.h" 
#include "util.h"

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

HashMap *hm_const = NULL;
HashMap *hm_var = NULL;
HashMap *hm_func = NULL;

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
    fprintf(stderr, "\n %serror:%s %s\n\n", COLOR_RED, COLOR_RESET, msg);
}

void print_expr_result(const char* line, const Expr *expr)
{
    double result = eval_expr(expr);

    if (expr->type == EXPR_ASSIGN) {
        printf("\n %s%.15g%s -> %s\n\n",
                COLOR_CYAN,
                result,
                COLOR_RESET,
                expr->assign.name);
    } else {
        printf("\n %s = %s%.15g%s\n\n",
                line,
                COLOR_CYAN,
                result,
                COLOR_RESET);
    }
}

void term_clear()
{
    printf(TERM_CLEAR);
}

int main(void)
{
    hm_const = hm_init(ARR_SIZE(constants));
    if (!hm_const) {
        print_error("failed to initialize hash table for constants");
        return 1;
    }

    for (size_t i = 0; i < ARR_SIZE(constants); ++i)
        hm_ins(hm_const, constants[i].name, &constants[i].value);

    hm_var = hm_init(16);
    if (!hm_var) {
        print_error("failed to initialize hash table for variables");
        return 2;
    }

    hm_func = hm_init(ARR_SIZE(constants));
    if (!hm_func) {
        print_error("failed to initialize hash table for functions");
        return 3; 
    }

    for (size_t i = 0; i < ARR_SIZE(builtins); ++i)
        hm_ins(hm_func, builtins[i].name, &builtins[i]);

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

        Expr *expr = parse_assignment(&parser);
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
    hm_free(hm_const);
    hm_free(hm_var);
    hm_free(hm_func);
    
    return 0;
}
