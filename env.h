#pragma once

typedef struct { char *key; Value *value; } Entry;

typedef struct Environment {
    int entries;
    int size;
    Entry *table;
    struct Environment *parent;
} Environment;

Environment *make_env(Environment *parent);
Value *get_env(Environment *env, char *key);
Environment *find_env(Environment *env, char *key);
void set_env(Environment *env, char *key, Value *value);
void define_env(Environment *env, char *key, Value *value);
void merge_env(Environment *dst, Environment *src);
