#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "parse.h"
#include "show.h"

#define HEAP_SIZE 100000

void *heap;
int alloc_index;

void init_heap() {
    heap = (void *) malloc(HEAP_SIZE);
    alloc_index = 0;
}

void destroy_heap() {
    free(heap);
}

void *allocate(int bytes) {
    void *result = heap + alloc_index;
    alloc_index += bytes;
    assert(alloc_index < HEAP_SIZE);
    return result;
}

Value *new_value() {
    Value *val = allocate(sizeof(Value));
    return val;
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
    char *name_copy = (char *) allocate(strlen(name) + 1);
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
    Pair *pair = allocate(sizeof(Pair));
    pair->fst = fst; pair->snd = snd;
    Value *val = new_value();
    val->t = PAIR; val->v.pair_value = pair;
    return val;
}

Value *make_lambda(Value *arguments, Value *body, Environment *env) {
    Lambda *lambda = allocate(sizeof(Lambda));
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