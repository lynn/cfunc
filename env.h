#pragma once

typedef struct { char *key; Value *value; } Entry;

typedef struct Environment {
    bool marked;
    int entries;
    int size;
    Entry *table;
    struct Environment *parent;
} Environment;

Environment *global_environment;
Environment *make_env(Environment *parent);
void destroy_env(Environment *env);
Value *get_env(Environment *env, char *key);
Environment *find_env(Environment *env, char *key);
void set_env(Environment *env, char *key, Value *value);
void define_env(Environment *env, char *key, Value *value);
void merge_env(Environment *dst, Environment *src);
