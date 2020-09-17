#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "value.h"

Environment *make_env(Environment *parent) {
    Environment *env = malloc(sizeof(Environment));
    env->entries = 0;
    env->size = 8;
    env->table = (Entry *) malloc(sizeof(Entry) * env->size);
    env->parent = parent;
    return env;
}

Value *get_env(Environment *env, char *key) {
    assert(env != NULL);
    assert(key != NULL);
    int i;
    for (i = 0; i < env->entries; i++) {
        if (!strcmp(key, env->table[i].key))
            return env->table[i].value;
    }
    return NULL;
}

Environment *find_env(Environment *env, char *key) {
    while (get_env(env, key) == NULL) {
        env = env->parent;
        if (env == NULL)
            return NULL;
    }
    return env;
}

void set_env(Environment *env, char *key, Value *value) {
    assert(env != NULL);
    assert(key != NULL);
    assert(value != NULL);

    int i;
    for (i = 0; i < env->size; i++) {
        if (!strcmp(key, env->table[i].key)) {
            env->table[i].value = value;
            return;
        }
    }
    fprintf(stderr, "Key \"%s\" not found during set!.\n", key);
    exit(1);
}

void define_env(Environment *env, char *key, Value *value) {
    assert(env != NULL);
    assert(key != NULL);
    assert(value != NULL);

    if (get_env(env, key) != NULL) {
        fprintf(stderr, "\"%s\" was defined twice.\n", key);
        exit(1);
    }

    env->table[env->entries].key = key;
    env->table[env->entries].value = value;
    env->entries++;
    if (env->entries >= env->size) {
        env->size *= 2;
        env->table = (Entry *) realloc(env->table, sizeof(Entry) * env->size);
    }
}

void destroy_env(Environment *env) {
    assert(env != NULL);
    free(env->table);
}

void merge_env(Environment *dst, Environment *src) {
    int i = 0;
    for (i = 0; i < src->entries; i++)
        define_env(dst, src->table[i].key, src->table[i].value);
}