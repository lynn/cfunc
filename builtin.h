#include <string.h>
#include "value.h"

bool from_bool(Value *x) { assert(x->t == BOOL); return x->v.bool_value; }
bool is_nil(Value *x) { assert(x->t == PAIR); return x->v.pair_value == NULL; }
Value *car(Value *x) { assert(x->t == PAIR); return x->v.pair_value->fst; }
Value *cdr(Value *x) { assert(x->t == PAIR); return x->v.pair_value->snd; }

Value *builtin_begin(Value *args) {
    // The args have all been evaluated in order.
    // Just return the final one:
    while (!is_nil(cdr(args))) args = cdr(args);
    return car(args);
}

Value *builtin_car(Value *args) {
    // Return the car of the first argument.
    return car(car(args));
}

Value *builtin_cdr(Value *args) {
    // Return the cdr of the first argument.
    return cdr(car(args));
}

Value *builtin_cons(Value *args) {
    return make_pair(car(args), car(cdr(args)));
}

Value *builtin_is_nil(Value *args) {
    Value *x = car(args);
    return make_bool(x->t == PAIR && x->v.pair_value == NULL);
}

Value *builtin_list(Value *args) {
    return args;
}

Value *builtin_plus(Value *args) {
    double sum = 0.0;
    while (!is_nil(args)) {
        assert(car(args)->t == NUMBER);
        sum += car(args)->v.number_value;
        args = cdr(args);
    }
    return make_number(sum);
}

Value *builtin_minus(Value *args) {
    double sum = 0.0;
    bool first = true;
    while (!is_nil(args)) {
        assert(car(args)->t == NUMBER);
        double v = car(args)->v.number_value;
        sum = first ? v : sum - v;
        first = false;
        args = cdr(args);
    }
    return make_number(sum);
}

Value *builtin_times(Value *args) {
    double product = 1.0;
    while (!is_nil(args)) {
        assert(car(args)->t == NUMBER);
        product *= car(args)->v.number_value;
        args = cdr(args);
    }
    return make_number(product);
}

Value *builtin_divide(Value *args) {
    double product = 1.0;
    bool first = true;
    while (!is_nil(args)) {
        assert(car(args)->t == NUMBER);
        double v = car(args)->v.number_value;
        product = first ? v : product / v;
        first = false;
        args = cdr(args);
    }
    return make_number(product);
}

#define BuiltinCompare(name, op) \
    Value *name(Value *args) { \
        assert(car(args)->t == NUMBER); \
        assert(car(cdr(args))->t == NUMBER); \
        bool equal = car(args)->v.number_value op car(cdr(args))->v.number_value; \
        return make_bool(equal); \
    }

BuiltinCompare(builtin_number_eq, ==)
BuiltinCompare(builtin_number_ne, !=)
BuiltinCompare(builtin_number_lt, <)
BuiltinCompare(builtin_number_gt, >)
BuiltinCompare(builtin_number_le, <=)
BuiltinCompare(builtin_number_ge, >=)

#undef BuiltinCompare

Value *builtin_number_equal(Value *args) {
    assert(car(args)->t == NUMBER);
    assert(car(cdr(args))->t == NUMBER);
    bool equal = car(args)->v.number_value == car(cdr(args))->v.number_value;
    return make_bool(equal);
}

Value *builtin_string_cat(Value *args) {
    assert(car(args)->t == STRING);
    assert(car(cdr(args))->t == STRING);
    char *s1 = car(args)->v.symbol_value;
    char *s2 = car(cdr(args))->v.symbol_value;
    int n1 = strlen(s1);
    int n2 = strlen(s2);
    char *buffer = (char *)malloc(n1 + n2 + 1);
    strcpy(buffer, s1);
    strcpy(buffer + n1, s2);
    return make_string(buffer);
}

Value *builtin_string_length(Value *args) {
    assert(car(args)->t == STRING);
    return make_number((double)strlen(car(args)->v.symbol_value));
}

Value *builtin_string_index(Value *args) {
    assert(car(args)->t == STRING);
    char *s = car(args)->v.symbol_value;
    size_t i = (size_t)car(cdr(args))->v.number_value;
    return make_number((double)s[i]);
}

Value *builtin_string_index(Value *args) {
    assert(car(args)->t == STRING);
    char *s = car(args)->v.symbol_value;
    size_t i = (size_t)car(cdr(args))->v.number_value;
    return make_number((double)s[i]);
}
