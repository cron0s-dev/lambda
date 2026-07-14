#ifndef FUNC_H
#define FUNC_H

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

typedef double (*BuiltinFunc)(double *args, size_t count);

typedef struct {
    const char *name;
    int arg_count;
    BuiltinFunc func;
} Builtin;

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

static double fn_deg(double * args, size_t count)
{
    return args[0] * 180.0 / M_PI;
}

static double fn_rad(double * args, size_t count)
{
    return args[0] * M_PI / 180.0;
}

static double fn_log(double *args, size_t count)
{
    if (count > 2)
        return 0.0;

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

static double fn_cbrt(double *args, size_t count)
{
    return cbrt(args[0]);
}

static double fn_root(double *args, size_t count)
{
    return pow(args[0], 1.0 / args[1]);
}

static double fn_hypot(double *args, size_t count)
{
    if (count < 2)
        return 0.0;

    double radicand = 0.0;

    for (size_t i = 0; i < count; i++)
       radicand += args[i] * args[i];

    return sqrt(radicand);
}

static double fn_gamma(double *args, size_t count)
{
    return tgamma(args[0] + 1.0);
}

static double fn_erf(double *args, size_t count)
{
    return erf(args[0]);
}

static double fn_mean(double *args, size_t count)
{
    if (count < 2)
        return 0.0;

    double sum = 0.0;

    for (size_t i = 0; i < count; i++)
       sum += args[i];

    return sum / count;
}

static double fn_rms(double *args, size_t count)
{
    if (count < 2)
        return 0.0;

    double sum = 0.0;

    for (size_t i = 0; i < count; i++)
       sum += args[i] * args[i];

    return sqrt(sum / count);
}

static double fn_sum(double *args, size_t count)
{
    if (count < 2)
        return 0.0;

    double sum = 0.0;

    for (size_t i = 0; i < count; i++)
       sum += args[i];

    return sum;
}

static double fn_sum_range(double *args, size_t count)
{
    int a = (int)args[1];
    int b = (int)args[2];
    return ((a - b + 1) * (a + b)) / 2.0;
}

static double fn_prod(double *args, size_t count)
{
    if (count < 2)
        return 0.0;

    double prod = 1.0;

    for (size_t i = 0; i < count; i++)
       prod *= args[i];

    return prod;
}

static double fn_prod_range(double *args, size_t count)
{
    double val = 1.0;
    size_t start = args[0];
    size_t end = args[1];

    for (size_t i = start; i <= end; i++)
        val *= i;

    return val;
}

static int gcd(int a, int b)
{
    a = abs(a);
    b = abs(b);

    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }

    return a;
}

static double fn_gcd(double *args, size_t count)
{
    double val = (int)args[0];

    for (size_t i = 1; i < count; i++)
        val = gcd(val, (int)args[i]);

    return val;
}

static double lcm(int a, int b)
{
    return abs(a / gcd(a, b) * b);
}

static double fn_lcm(double *args, size_t count)
{
    int val = (int)args[0];

    for (size_t i = 1; i < count; i++)
        val = lcm(val, args[i]);

    return val;
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

    {"deg", 1, fn_deg},
    {"rad", 1, fn_rad},

    {"log",   -1, fn_log},
    {"ln",    1, fn_ln},
    {"log2",  1, fn_log2},

    {"pow",   2, fn_pow},
    {"sqrt",  1, fn_sqrt},
    {"cbrt",  1, fn_cbrt},
    {"root",  2, fn_root},
    {"hypot", -1, fn_hypot},

    {"gamma",  1, fn_gamma},
    {"erf", 1, fn_erf},

    {"sum", -1, fn_sum},
    {"sum_range", 2, fn_sum_range},
    {"prod_range", 2, fn_prod_range},
    {"prod", -1, fn_prod},
    {"mean", -1, fn_mean},
    {"rms", -1, fn_rms},
    {"gcd", -1, fn_gcd},
    {"lcm", -1, fn_lcm},
};

#endif
