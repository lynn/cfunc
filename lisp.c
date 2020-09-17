#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "show.h"
#include "parse.h"
#include "env.h"

bool from_bool(Value *x) {
    assert(x->t == BOOL);
    return x->v.bool_value;
}

bool pair_is_nil(Value *x) {
    assert(x->t == PAIR);
    return x->v.pair_value == NULL;
}

Value *car(Value *x) {
    assert(x->t == PAIR);
    return x->v.pair_value->fst;
}

Value *cdr(Value *x) {
    assert(x->t == PAIR);
    return x->v.pair_value->snd;
}

Value *builtin_car(Value *args) {
    return car(car(args));
}

Value *builtin_cdr(Value *args) {
    return cdr(car(args));
}

Value *builtin_cons(Value *args) {
    return make_pair(car(args), car(cdr(args)));
}

Value *builtin_plus(Value *args) {
    double sum = 0.0;
    while (!pair_is_nil(args)) {
        assert(car(args)->t == NUMBER);
        sum += car(args)->v.number_value;
        args = cdr(args);
    }
    return make_number(sum);
}

Environment *make_global_env() {
    Environment *e = make_env(NULL);
    define_env(e, "car", make_builtin(builtin_car));
    define_env(e, "cdr", make_builtin(builtin_cdr));
    define_env(e, "cons", make_builtin(builtin_cons));
    define_env(e, "+", make_builtin(builtin_plus));
    return e;
}

#define IS_KEYWORD(f, k) ((f)->t == SYMBOL && !strcmp((f)->v.symbol_value, k))

Environment *lambda_env(Lambda *f, Value *args, Environment *parent) {
    Environment *lenv;
    lenv = make_env(parent);
    Value *val;
    for (val = f->arguments; !pair_is_nil(val); val = cdr(val)) {
        assert(car(val)->t == SYMBOL);
        define_env(lenv, car(val)->v.symbol_value, car(args));
        args = cdr(args);
    }
    return lenv;
}

Value *map_eval(Value *exp, Environment *env);

Value *eval(Value *exp, Environment *env) {
    char *s;
    Value *f, *tail;

    switch (exp->t) {
    case BOOL:
    case NUMBER:
    case STRING:
    case BUILTIN:
        return exp;
    case SYMBOL:
        s = exp->v.symbol_value;
        return get_env(find_env(env, s), s);
    case PAIR:
        if (pair_is_nil(exp)) /* nil constant */
            return exp;

        /* Apply a function. */
        f = car(exp); tail = cdr(exp);
        if (IS_KEYWORD(f, "if")) {
            return eval(from_bool(eval(car(tail), env))
                        ? car(cdr(tail))
                        : car(cdr(cdr(tail))), env);
        } else if (IS_KEYWORD(f, "lambda")) {
            return make_lambda(car(tail), car(cdr(tail)), env);
        } else {
            f = eval(f, env);
            if (f->t == BUILTIN) {
                assert(tail->t == PAIR);
                return (*f->v.builtin_value)(map_eval(tail, env));
            } else if (f->t == LAMBDA) {
                return eval(f->v.lambda_value->body,
                            lambda_env(f->v.lambda_value, tail, env));
            } else {
                printf("Tried to call non-function: ");
                print_value(f, true); puts("");
                exit(1);
            }
        }
    default:
        puts("???");
        exit(1);
    }
}

Value *map_eval(Value *exp, Environment *env) {
    if (pair_is_nil(exp))
        return exp;
    else
        return make_pair(eval(car(exp), env),
                         map_eval(cdr(exp), env));
}

int main(int argc, char **argv) {
    init_heap();
    Environment *global = make_global_env();
    Value *code = parse_value("((lambda (x) (+ x x)) 5)", NULL);
    // Value *code = parse_value("((1 2))", NULL);
    print_value(code, true); puts("");
    print_value(eval(code, global), true); puts("");
    destroy_heap();
    return 0;
}
