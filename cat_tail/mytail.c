/* FILE: mytail.c
 * Tiantian Fang CS107
 *
 * This program prints the final N lines of the input in order. The value of N defaults to 10, but can also be set as a command-line flag -N. This program only reads one file.
 */
#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NUM_LINES 100000
#define DEFAULT_NUM_LINES 10

/* FUNCTION: print_last_n
 *
 * Parameters:
 *    fp - file to be read
 *    n  - length of the sliding window 
 *
 * Returns: NA
 *
 * This function prints the last n lines in the file.
 */
void print_last_n(FILE *fp, int n) {
    char *arr[n];
    int line = 0;
    int start = 0;
    char* this;
    while ((this = read_line(fp)) != NULL) {
        if (line >= n) {
            free(arr[line%n]);
            //updates the start position in the sliding window
            start = (start + 1)%n;
        }
        arr[line%n] = strdup(this);
        assert(arr[line%n] != NULL);
        line ++;
        free(this);
    }
    //takes into account the situation when the sliding window is not filled
    int min = (line < n) ? line : n;
    for (int i = start; i < start + min; i ++) {
        printf("%s\n", arr[i%n]);
        free(arr[i%n]);
    }
}

// ------- DO NOT EDIT ANY CODE BELOW THIS LINE (but do add comments!)  -------

int convert_arg(const char *str, int max) {
    char *end;
    long n = strtol(str, &end, 10);
    if (*end != '\0') {
        error(1, 0, "Invalid number '%s'", str);
    }
    if (n < 1 || n > max) {
        error(1, 0, "%s is not within the acceptable range [%d, %d]", str, 1, max);
    }
    return n;
}

int main(int argc, char *argv[]) {
    int num_lines = DEFAULT_NUM_LINES;

    if (argc > 1 && argv[1][0] == '-') {
        // processes the command-line flag
        num_lines = convert_arg(argv[1] + 1, MAX_NUM_LINES);
        argv++;
        argc--;
    }

    FILE *fp;
    if (argc == 1) {
        fp = stdin;
    } else {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            error(1, errno, "cannot access '%s'", argv[1]);
        }
    }
    print_last_n(fp, num_lines);
    fclose(fp);
    return 0;
}
