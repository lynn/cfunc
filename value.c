#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "parse.h"
#include "show.h"
#include "gc.h"

Value *heap[HEAP_SIZE];
int heap_index = 0;
int max_heap_index = 0;
int heap_values = 0;

Value *new_value() {
    Value *val = malloc(sizeof(Value));
    val->marked = false;

    // Find an empty spot on the heap.
    while (heap[heap_index] != NULL) {
        heap_index++;
        if (heap_index >= HEAP_SIZE) {
            fprintf(stderr, "out of memory!\n");
            exit(1);
        }
    }
    heap[heap_index] = val;
    if (heap_index > max_heap_index)
        max_heap_index = heap_index;
    heap_values++;
    printf("%d\n", heap_values);
    if (heap_values >= 50000) {
        mark_and_sweep();
    }

    return val;
}

void free_value(Value *val) {
    switch (val->t) {
        case BOOL:
        case NUMBER:
        case BUILTIN:
            break;
        case SYMBOL:
        case STRING:
            free(val->v.symbol_value);
            break;
        case PAIR:
            if (val->v.pair_value != NULL) {
                free(val->v.pair_value);
            }
            break;
        case LAMBDA:
            free(val->v.lambda_value);
            break;
    }
    free(val);
}

Value *make_nil() {
    Value *val = new_value();
    val->t = PAIR; val->v.pair_value = NULL;
    return val;
}

Value *make_bool(bool x) {
    Value *val = new_value();
    val->t = BOOL; val->v.bool_value = x;
    return val;
}

Value *make_number(double x) {
    Value *val = new_value();
    val->t = NUMBER; val->v.number_value = x;
    return val;
}

Value *make_symbol(char *name) {
    char *name_copy = (char *) malloc(strlen(name) + 1);
    strcpy(name_copy, name);
    Value *val = new_value();
    val->t = SYMBOL; val->v.symbol_value = name_copy;
    return val;
}

Value *make_string(char *s) {
    Value *val = make_symbol(s);
    val->t = STRING;
    return val;
}

Value *make_pair(Value *fst, Value *snd) {
    Pair *pair = malloc(sizeof(Pair));
    pair->fst = fst; pair->snd = snd;
    Value *val = new_value();
    val->t = PAIR; val->v.pair_value = pair;
    return val;
}

Value *make_lambda(Value *arguments, Value *body, Environment *env) {
    Lambda *lambda = malloc(sizeof(Lambda));
    lambda->arguments = arguments;
    lambda->body = body;
    lambda->env = env;
    Value *val = new_value();
    val->t = LAMBDA; val->v.lambda_value = lambda;
    return val;
}

Value *make_builtin(Value *(*func)(Value *)) {
    Value *val = new_value();
    val->t = BUILTIN; val->v.builtin_value = func;
    return val;
}
