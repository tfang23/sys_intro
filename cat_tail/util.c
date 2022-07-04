/* FILE: util.c
 * Tiantian Fang CS107
 *
 * This program has one function read_line which reads the next line from a file.
 */
#include "samples/prototypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* initial allocation will be for min size, if not big enough, doubles
 * to 64, then 128, then 256, etc. as needed to accommodate the entire line
 * resize-as-you-go, doubling each time.
 */
#define MINIMUM_SIZE 32

/* FUNCTION: read_line
 *
 * Parameter:
 *    fp - file to be read
 *
 * Returns: the next line having been read
 *
 * This function takes in a file, reads the next line from the file, and returns this line in a dynamically-allocated and null-terminated string.
 */
char *read_line(FILE *fp) {
    char *line = malloc(MINIMUM_SIZE);
    assert(line != NULL);
    char *first = fgets(line, MINIMUM_SIZE, fp);
    //if reaching EOF immediately, free the allocated space and return NULL
    if (first == NULL) {
        free(line);
        return NULL;
    }
    int size = MINIMUM_SIZE;
    while (1) {
        if (strlen(line) < size - 1) {
            if (*(line + strlen(line) - 1) == '\n') {
                *(line + strlen(line) - 1) = '\0';
            }
            return line;
        }
        size *= 2;
        line = realloc(line, size);
        assert(line != NULL);
        //if reaching EOF, free the newly allocated space and return the line already read
        if (fgets(line + strlen(line), strlen(line) + 2, fp) == NULL) {
            line = realloc(line, size/2);
            assert(line != NULL);
            return line;
        }
    }   
    return line;
}
