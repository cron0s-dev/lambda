#include "eval.h"
#include "ast.h"

#include <math.h>
#include <string.h>

typedef double (*BuiltinFunc)(double *args, size_t count);

typedef struct {
    const char *name;
    size_t arg_count;
    BuiltinFunc func;
} Builtin;

static double fn_sin(double *args, size_t count)
{
    return sin(args[0]);
}

static double fn_cos(double *args, size_t count)
{
    return cos(args[0]);
}

static double fn_tan(double *args, size_t count)
{
    return tan(args[0]);
}

static double fn_asin(double *args, size_t count)
{
    return asin(args[0]);
}

static double fn_acos(double *args, size_t count)
{
    return acos(args[0]);
}

static double fn_atan(double *args, size_t count)
{
    return atan(args[0]);
}

static double fn_atan2(double *args, size_t count)
{
    return atan2(args[0], args[1]);
}

static double fn_csc(double *args, size_t count)
{
    return 1.0 / sin(args[0]);
}

static double fn_sec(double *args, size_t count)
{
    return 1.0 / cos(args[0]);
}

static double fn_cot(double *args, size_t count)
{
    return 1.0 / tan(args[0]);
}

static double fn_sinh(double *args, size_t count)
{
    return sinh(args[0]);
}

static double fn_cosh(double *args, size_t count)
{
    return cosh(args[0]);
}

static double fn_tanh(double *args, size_t count)
{
    return tanh(args[0]);
}

static double fn_asinh(double *args, size_t count)
{
    return asinh(args[0]);
}

static double fn_acosh(double *args, size_t count)
{
    return acosh(args[0]);
}

static double fn_atanh(double *args, size_t count)
{
    return atanh(args[0]);
}

static double fn_log(double *args, size_t count)
{
    return log10(args[0]);
}

static double fn_logn(double *args, size_t count)
{
    if (count == 1)
        return log10(args[0]);

    return log(args[0]) / log(args[1]);
}

static double fn_ln(double *args, size_t count)
{
    return log(args[0]);
}

static double fn_log2(double *args, size_t count)
{
    return log2(args[0]);
}

static double fn_pow(double *args, size_t count)
{
    return pow(args[0], args[1]);
}

static double fn_sqrt(double *args, size_t count)
{
    return sqrt(args[0]);
}

static double fn_root(double *args, size_t count)
{
    return pow(args[0], 1.0 / args[1]);
}

static Builtin builtins[] = {
    {"sin",   1, fn_sin},
    {"cos",   1, fn_cos},
    {"tan",   1, fn_tan},

    {"asin",  1, fn_asin},
    {"acos",  1, fn_acos},
    {"atan",  1, fn_atan},
    {"atan2", 2, fn_atan2},

    {"csc",   1, fn_csc},
    {"sec",   1, fn_sec},
    {"cot",   1, fn_cot},

    {"sinh",  1, fn_sinh},
    {"cosh",  1, fn_cosh},
    {"tanh",  1, fn_tanh},

    {"asinh", 1, fn_asinh},
    {"acosh", 1, fn_acosh},
    {"atanh", 1, fn_atanh},

    {"log",   1, fn_log},
    {"logn",  2, fn_logn},
    {"ln",    1, fn_ln},
    {"log2",  1, fn_log2},

    {"pow",   2, fn_pow},
    {"sqrt",  1, fn_sqrt},
    {"root",  2, fn_root},
};

typedef struct {
    const char *name;
    double value;
} Constant;

static Constant constants[] = {
    {"pi",  M_PI},
    {"e",   M_E},
    {"tau", 2 * M_PI},

    {"phi", 1.618033988749895},
    {"sqrt2", 1.4142135623730951},
    {"sqrt3", 1.2599210498948732},

    {"c",  299792458.0},
    {"G",  6.67430e-11},
    {"h",  6.62607015e-34},
    {"kB", 1.380649e-23},
    {"Na", 6.02214076e23},

    {"deg", 180.0 / M_PI},
    {"rad", M_PI / 180.0},
};

double eval_expr(const Expr *expr)
{
    switch (expr->type) {
        case EXPR_NUM:
            return expr->num;

        case EXPR_UNARY:
            return eval_unary(expr);

        case EXPR_BINARY:
            return eval_binary(expr);

        case EXPR_CALL:
            return eval_call(expr);

        case EXPR_IDENT:
            return eval_ident(expr);
            break;
    }

    return 0.0;
}

double eval_binary(const Expr *expr)
{
    double left = eval_expr(expr->binary.left);
    double right = eval_expr(expr->binary.right);

    switch (expr->binary.op) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            return left / right;
        case '%':
            return fmod(left, right);
        case '^':
            return pow(left, right);
    }

    return 0.0;
}

double eval_call(const Expr *expr)
{
    for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
        Builtin *builtin = &builtins[i];

        if (strcmp(expr->call.name, builtin->name) == 0) {

            if (expr->call.arg_count != builtin->arg_count)
                return 0.0;

            double args[expr->call.arg_count];

            for (size_t j = 0; j < expr->call.arg_count; j++)
                args[j] = eval_expr(expr->call.args[j]);

            return builtin->func(args, expr->call.arg_count);
        }
    }

    return 0.0;
}

double eval_unary(const Expr *expr)
{
    double operand = eval_expr(expr->unary.operand);

    switch (expr->unary.op) {
        case '+':
            return operand;

        case '-':
            return -operand;
    }

    return 0.0;
}

double eval_ident(const Expr *expr)
{
    for (size_t i = 0; i < sizeof(constants) / sizeof(constants[0]); i++) {
        if (strcmp(expr->ident, constants[i].name) == 0)
            return constants[i].value;
    }

    return 0.0;
}
