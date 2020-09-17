#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "show.h"
#include "parse.h"
#include "env.h"
#include "gc.h"
#include "builtin.h"

Environment *make_global_env() {
    Environment *e = make_env(NULL);
    define_env(e, "begin", make_builtin(builtin_begin));
    define_env(e, "car", make_builtin(builtin_car));
    define_env(e, "cdr", make_builtin(builtin_cdr));
    define_env(e, "cons", make_builtin(builtin_cons));
    define_env(e, "nil?", make_builtin(builtin_is_nil));
    define_env(e, "list", make_builtin(builtin_list));
    define_env(e, "+", make_builtin(builtin_plus));
    define_env(e, "-", make_builtin(builtin_minus));
    define_env(e, "*", make_builtin(builtin_times));
    define_env(e, "/", make_builtin(builtin_divide));
    define_env(e, "=", make_builtin(builtin_number_eq));
    define_env(e, "!=", make_builtin(builtin_number_ne));
    define_env(e, "<", make_builtin(builtin_number_lt));
    define_env(e, ">", make_builtin(builtin_number_gt));
    define_env(e, "<=", make_builtin(builtin_number_le));
    define_env(e, ">=", make_builtin(builtin_number_ge));
    define_env(e, "string-cat", make_builtin(builtin_string_cat));
    define_env(e, "string-length", make_builtin(builtin_string_length));
    define_env(e, "string-index", make_builtin(builtin_string_index));
    return e;
}

#define IS_KEYWORD(f, k) ((f)->t == SYMBOL && !strcmp((f)->v.symbol_value, k))

Environment *lambda_env(Lambda *f, Value *args, Environment *parent) {
    Environment *lenv;
    lenv = make_env(parent);
    Value *val;
    for (val = f->arguments; !is_nil(val); val = cdr(val)) {
        assert(car(val)->t == SYMBOL);
        define_env(lenv, car(val)->v.symbol_value, car(args));
        args = cdr(args);
    }
    return lenv;
}

Value *map_eval(Value *exp, Environment *env);

Value *eval(Value *exp, Environment *env) {
    char *s;
    // puts("Evaluating:"); print_value(exp,true); puts("");
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
        if (is_nil(exp)) /* nil constant */
            return exp;

        /* Apply a function. */
        f = car(exp); tail = cdr(exp);
        if (IS_KEYWORD(f, "if")) {
            bool b = from_bool(eval(car(tail), env));
            return eval(b ? car(cdr(tail)) : car(cdr(cdr(tail))), env);
        } else if (IS_KEYWORD(f, "cond")) {
            while (!is_nil(tail)) {
                bool b = from_bool(eval(car(car(tail)), env));
                if (b) return eval(car(cdr(car(tail))), env);
                tail = cdr(tail);
            }
            return make_nil();
        } else if (IS_KEYWORD(f, "lambda")) {
            return make_lambda(car(tail), car(cdr(tail)), env);
        } else if (IS_KEYWORD(f, "quote")) {
            return car(tail);
        } else if (IS_KEYWORD(f, "define")) {
            char *key = car(tail)->v.symbol_value;
            Value *val = eval(car(cdr(tail)), env);
            define_env(env, key, val);
            return make_nil();
        } else {
            Value *fe = eval(f, env);
            if (fe->t == BUILTIN) {
                assert(tail->t == PAIR);
                return (*fe->v.builtin_value)(map_eval(tail, env));
            } else if (fe->t == LAMBDA) {
                Environment *lenv = lambda_env(fe->v.lambda_value, map_eval(tail, env), env);
                Value *result = eval(fe->v.lambda_value->body, lenv);
                destroy_env(lenv);
                return result;
            } else {
                printf("Tried to call non-function: ");
                print_value(fe, true); puts("");
                exit(1);
            }
        }
    default:
        puts("???");
        exit(1);
    }
}

Value *map_eval(Value *exp, Environment *env) {
    if (is_nil(exp)) {
        return exp;
    } else {
        Value *v = eval(car(exp), env);
        return make_pair(v, map_eval(cdr(exp), env));
    }
}

int main(int argc, char **argv) {
    global_environment = make_global_env();
    printf("sizeof(Value) = %lu\n", sizeof(Value));
    char *buffer = 0;
    long length;
    FILE *f = fopen(argv[1], "rb"); if (!f) exit(1);
    fseek(f, 0, SEEK_END); length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = malloc(length); if (!buffer) exit(1);
    fread(buffer, 1, length, f);
    fclose(f);
    Value *code = parse_value(buffer, NULL);
    print_value(code, true); puts("");
    // Stick it to the environment so it doesn't get GC'ed.
    define_env(global_environment, "___code", code);
    print_value(eval(code, global_environment), true); puts("");
    mark_and_sweep();
    puts("Done.");
    destroy_env(global_environment);
    return 0;
}
