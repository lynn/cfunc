#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "show.h"

void print_value(Value *val, bool outer) {
    assert(val != NULL);
    switch (val->t) {
        case BOOL:
            printf("\x1B[33m%s\x1B[0m", val->v.bool_value ? "#t" : "#f");
            break;
        case NUMBER:
            printf("\x1B[32m%g\x1B[0m", val->v.number_value);
            break;
        case SYMBOL:
            printf("\x1B[31m%s\x1B[0m", val->v.symbol_value);
            break;
        case STRING:
            printf("\x1B[34m\"%s\"\x1B[0m", val->v.symbol_value);
            break;
        case PAIR:
            if (val->v.pair_value == NULL) {
                fputs("\x1B[33;1m#nil\x1B[0m", stdout);
            } else {
                if (outer) fputs("(", stdout);
                print_value(val->v.pair_value->fst, true);
                Value *snd = val->v.pair_value->snd;
                assert(snd != NULL);
                if (snd->t == PAIR) {
                    if (snd->v.pair_value != NULL) {
                        fputs(" ", stdout);
                        print_value(snd, false);
                    }
                } else {
                    fputs(" . ", stdout);
                    print_value(snd, true);
                }
                if (outer) fputs(")", stdout);
            }
            break;
        case LAMBDA:
            fputs("\x1B[35;1m<lambda>\x1B[0m", stdout);
            break;
        case BUILTIN:
            fputs("\x1B[35;1m<builtin>\x1B[0m", stdout);
            break;
    }
}

int sprint_value(char *s, Value *val, bool outer) {
    /* Returns the number of characters written, excluding '\0'. */
    assert(s != NULL);
    assert(val != NULL);
    switch (val->t) {
        case BOOL:
            return sprintf(s, val->v.bool_value ? "#t" : "#f");
        case NUMBER:
            return sprintf(s, "%g", val->v.number_value);
        case SYMBOL:
            return sprintf(s, "%s", val->v.symbol_value);
        case STRING:
            return sprintf(s, "\"%s\"", val->v.symbol_value);
        case PAIR:
            if (val->v.pair_value == NULL) {
                return sprintf(s, "#nil");
            } else {
                char *start = s;
                if (outer) s += sprintf(s, "(");
                s += sprint_value(s, val->v.pair_value->fst, true);
                Value *snd = val->v.pair_value->snd;
                assert(snd != NULL);
                if (snd->t == PAIR) {
                    if (snd->v.pair_value != NULL) {
                        s += sprintf(s, " ");
                        s += sprint_value(s, snd, false);
                    }
                } else {
                    s += sprintf(s, " . ");
                    s += sprint_value(s, snd, true);
                }
                if (outer) s += sprintf(s, ")");
                return (s - start);
            }
        case LAMBDA:
            return sprintf(s, "<lambda>");
        case BUILTIN:
            return sprintf(s, "<builtin>");
        default:
            return 0;
    }
}

