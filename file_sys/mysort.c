/* FILE: mysort.c
 * Tiantian Fang CS107
 *
 * This file contains my version of sort.
 */
#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE_LEN 4096
#define MIN_NLINES 100

typedef int (*cmp_fn_t)(const void *p, const void *q);

/* FUNCTION: cmp_pstr
 * A comparison function that compares strings in lexicographic order in the same way as strcmp.
 */
int cmp_pstr(const void *p, const void *q) {
    const char *first = *(const char **)p;
    const char *second = *(const char **)q;
    return strcmp(first, second);
}

/* FUNCTION: cmp_pstr_len
 * A comparison function that compares strings in terms of increasing line length.
 */
int cmp_pstr_len(const void *p, const void *q) {
    return strlen(*(const char **)p) - strlen(*(const char **)q);
}

/* FUNCTION: cmp_pstr_numeric
 * A comparison function that compares the numeric value of each line.
 */
int cmp_pstr_numeric(const void *p, const void *q) {
    return atoi(*(const char **)p) - atoi(*(const char **)q);
}

/* FUNCTION: sort_lines
 *
 * Parameters:
 *     fp - the inputted file
 *     cmp - the comparison function
 *     uniq - whether we only retain unique lines
 *     reverse - whether we sort in reverse order
 *
 * This function sorts lines in the file into respective order according to the command line flags.
 */
void sort_lines(FILE *fp, cmp_fn_t cmp, bool uniq, bool reverse) {   
    char this_line[MAX_LINE_LEN];
    int size = MIN_NLINES;
    char **arr = malloc(size * sizeof(char *));
    assert(arr != NULL);
    size_t nused = 0;
    
    while (fgets(this_line, MAX_LINE_LEN, fp) != NULL) {
        char *line = strdup(this_line);
        assert(line != NULL);
        *(line + strlen(line) - 1) = '\0';

        //increase memory allocation if necessary
        if (nused + 1 == size) {
            size *= 2;
            arr = realloc(arr, size * (sizeof(char *)));
            assert(arr != NULL);
        }

        //handles the situation where we only retain unique lines
        if (uniq) {
            size_t old_nused = nused;
            binsert(&line, arr, &nused, sizeof(char *), cmp);
            if (nused == old_nused) {
                free(line);
            }
        } else {
            arr[nused] = line;
            nused ++;
        }
    }

    if (!uniq) {
        qsort(arr, nused, sizeof(char *), cmp);
    }

    for (int i = 0; i < nused; i++) {
        if (reverse) {
            printf("%s\n", arr[nused - i - 1]);
            free(arr[nused - i - 1]);
        } else {
            printf("%s\n", arr[i]);
            free(arr[i]);
        }
    }
    free(arr);
}

int main(int argc, char *argv[]) {
    cmp_fn_t cmp = cmp_pstr;
    bool uniq = false;
    bool reverse = false;

    int opt = getopt(argc, argv, "lnru");
    while (opt != -1) {
        if (opt == 'l') {
            cmp = cmp_pstr_len;
        } else if (opt == 'n') {
            cmp = cmp_pstr_numeric;
        } else if (opt == 'r') {
            reverse = true;
        } else if (opt == 'u') {
            uniq = true;
        } else {
            return 1;
        }

        opt = getopt(argc, argv, "lnru");
    }

    FILE *fp = stdin;
    if (optind < argc) {
        fp = fopen(argv[optind], "r");
        if (fp == NULL) {
            error(1, 0, "cannot access %s", argv[optind]);
        }
    }
    sort_lines(fp, cmp, uniq, reverse);
    fclose(fp);
    return 0;
}
