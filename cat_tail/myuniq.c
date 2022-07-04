/* FILE: myuniq.c
 * Tiantian Fang CS107
 *
 * This program outputs the provided input with all duplicate lines removed. Only one copy of each unique line is printed. This program always prefixes each line of output with its count of occurrences in the input. 
 */
#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* initial estimate of number of uniq lines
 * resize-as-you-go, add in increments of 100
 */
#define ESTIMATE 100

/* FUNCTION: print_uniq_lines
 *
 * Parameters:
 *    fp - file to be read
 *
 * Returns: NA
 *
 * This function prints out unique lines in order and prefixes each line with its frequency of occurrence in the file.
 */
void print_uniq_lines(FILE *fp) {
    //creates a struct of a unique line and its frequency
    typedef struct my_struct {
        int freq;
        char* line;
    } my_struct;
    struct my_struct *arr = malloc(ESTIMATE * sizeof(struct my_struct));
    assert(arr != NULL);
    int size = ESTIMATE;
    char *this;
    int max_index = 0;    
    while ((this = read_line(fp)) != NULL) {       
        bool found = false;
        //if a unique line comes up again, increase its frequency
        for (int i = 0; i < max_index; i++) {
            if (strcmp(this, arr[i].line) == 0) {
                arr[i].freq ++;
                found = true;
                break;
            }
        }
        if (!found) {            
            //increase the size if necessary
            if (max_index >= size) {
                size += ESTIMATE;
                arr = realloc(arr, (size * sizeof(struct my_struct)));
                assert(arr != NULL);
            }         
            //if encounters a new unique line, add it to the array and note that it occurs once
            arr[max_index].freq = 1;
            arr[max_index].line = strdup(this);
            assert(arr[max_index].line != NULL);
            max_index ++;
        }
        free(this);
    }
    for (int j = 0; j < max_index; j++) {
        printf("%7d %s\n", arr[j].freq, arr[j].line);
        free(arr[j].line);
    }
    free(arr);
}
   


// ------- DO NOT EDIT ANY CODE BELOW THIS LINE (but do add comments!)  -------

int main(int argc, char *argv[]) {
    FILE *fp;

    if (argc == 1) {
        fp = stdin;
    } else {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            error(1, errno, "cannot access '%s'", argv[1]);
        }
    }
    print_uniq_lines(fp);
    fclose(fp);
    return 0;
}
