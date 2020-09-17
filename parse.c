#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "value.h"
#include "parse.h"

char token[100];

char *next_token(char *s) {
    if (*s == '\0')
        return NULL;

    while (isspace((unsigned char) *s)) s++;
    if (*s == '(' || *s == ')') {
        token[0] = *s++;
        token[1] = '\0';
        return s;
    } else {
        int i = 0; char c;
        while (!isspace((unsigned char) *s) && *s != '(' && *s != ')') {
            c = token[i++] = *s++;
            if (c == '"') {
                while (*s != '"')
                    token[i++] = *s++;
                /* Skip the closing quote; don't write it to the token. */
                s++;
            }
        }
        token[i] = '\0';
        return s;
    }
}

/**
 * Parse an S-expression pair, and optionally report where it ends.
 * (If end != NULL, we store the "end of this pair" char-pointer in *end.)
 *
 */
Value *parse_pair(char *s, char **end) {
    /* We are *inside* a (, so: */
    Value *fst = parse_value(s, &s);
    char *s2 = next_token(s);

    if (!strcmp(token, ".")) {
        /* Read "fst . snd)" */
        Value *snd = parse_value(s2, &s);
        s = next_token(s);
        assert(!strcmp(token, ")"));
        if (end != NULL)
            *end = s;
        return make_pair(fst, snd);
    } else if (!strcmp(token, ")")) {
        /* Read "fst)" */
        if (end != NULL)
            *end = s2;
        return make_pair(fst, make_nil());
    } else {
        /* Read "fst snd ...)" */
        return make_pair(fst, parse_pair(s, end));
    }
}

Value *parse_value(char *s, char **end) {
    s = next_token(s);
    if (!strcmp(token, "(")) {
        return parse_pair(s, end);
    }
    if (end != NULL)
        *end = s;
    if (!strcmp(token, "#nil"))
        return make_nil();
    else if (!strcmp(token, "#t"))
        return make_bool(true);
    else if (!strcmp(token, "#f"))
        return make_bool(false);
    else if (isdigit((unsigned char) token[0])
             || (token[0] == '-' && isdigit((unsigned char) token[1])))
        return make_number(strtof(token, NULL));
    else if (token[0] == '"')
        return make_string(token + 1);
    else
        return make_symbol(token);
}
