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
    // Mathematical constants
    {"e",   M_E},
    {"pi",  M_PI},
    {"tau", 2 * M_PI},
    {"ln2", M_LN2},
    {"ln10", M_LN10},
    {"sqrt2", 1.4142135623730951},
    {"sqrt3", 1.2599210498948732},
    {"phi", 1.618033988749895},

    // Universal constants

    {"c",  299792458.0},
    {"G",  6.67430e-11},
    {"h",  6.62607015e-34},
    {"hbar", 1.054571817e-34},
    {"kB", 1.380649e-23},
    {"Na", 6.02214076e23},
    {"R", 8.314462618},
    {"e_charge", 1.602176634e-19},
    {"epsilon0", 8.8541878128e-12},
    {"mu0", 1.25663706212e-6},
    {"M_sun", 1.98847e30},
    {"R_sun", 6.957e8},

    // Astronomical constants

    {"M_earth", 5.9722e24},
    {"R_earth", 6.371e6},
    {"M_moon", 7.342e22},
    {"R_moon", 1.7374e6},
    {"AU", 1.495978707e11},
    {"ly", 9.460730472e15},
    {"pc", 3.085677581e16},

    {"a0", 5.29177210903e-11},
    {"alpha", 7.2973525693e-3},
    {"m_electron", 9.1093837139e-31},
    {"m_proton", 1.67262192595e-27},
    {"m_neutron", 1.67492750056e-27},

    {"sigma", 5.670374419e-8},
    {"g", 9.80665},

    {"atm", 101325.0},
    {"bar", 100000.0},
    {"coulomb", 8.9875517923e9},
    {"Z0", 376.730313668},

    {"amu", 1.66053906660e-27},
    {"F", 96485.33212},

    // {"inch", 2.54},
    // {"foot", 0.3048},
    // {"mile", 1609.344},
    {"zero_C", 273.15},

    {"rad", 180.0 / M_PI},
    {"deg", M_PI / 180.0},
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

static double fn_exp(double *args, size_t count)
{
    return exp(args[0]);
}

static double fn_exp2(double *args, size_t count)
{
    return exp2(args[0]);
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
    return tgamma(args[0]);
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

static double fn_abs(double *args, size_t count)
{
    return fabs(args[0]);
} 

static double fn_round(double *args, size_t count)
{
    return round(args[0]);
} 

static double fn_floor(double *args, size_t count)
{
    return floor(args[0]);
} 

static double fn_ceil(double *args, size_t count)
{
    return ceil(args[0]);
} 

static double fn_trunc(double *args, size_t count)
{
    return trunc(args[0]);
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

static double fn_fahrenheit(double *args, size_t count)
{
    return args[0] * 9.0 / 5.0 + 32.0;
}

static double fn_celsius(double *args, size_t count)
{
    return (args[0] - 32.0) * 5.0 / 9.0;
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

    {"exp", 1, fn_exp},
    {"exp2", 1, fn_exp2},

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

    {"abs", 1, fn_abs},
    {"round", 1, fn_round},
    {"floor", 1, fn_floor},
    {"ceil", 1, fn_ceil},
    {"trunc", 1, fn_trunc},

    {"sum", -1, fn_sum},
    {"sum_range", 2, fn_sum_range},
    {"prod_range", 2, fn_prod_range},
    {"prod", -1, fn_prod},
    {"mean", -1, fn_mean},
    {"rms", -1, fn_rms},
    {"gcd", -1, fn_gcd},
    {"lcm", -1, fn_lcm},

    {"celsius", 1, fn_celsius},
    {"fahrenheit", 1, fn_fahrenheit},
};

#endif
