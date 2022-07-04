/* File: mywhich.c
 * Tiantian Fang CS107
 * -------------------
 *
 * This program is a version of the Unix which command, a utlity used to locate and identify executable programs to run. Mywhich considers the directories in the order they are listeded in MYPATH (or PATH if the environment does not contain value for MYPATH) and stops at the first one that contains a matching executable. Mywhich invoked with no arguments prints the directories in the search path.
 */
#include "samples/prototypes.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
//includes unistd to use access()
#include <unistd.h>


int main(int argc, char *argv[], const char *envp[]) {
    const char *searchpath = get_env_value(envp, "MYPATH");
    if (searchpath == NULL) {
        //if the environment does not contain a value for MYPATH, the program uses PATH instead
        searchpath = get_env_value(envp, "PATH");
    }
    //creates an array of char of the maximum permitted size of file paths for storing the tokenized paths
    char dir[PATH_MAX];
    const char *remaining = searchpath;
    if (argc == 1) {
        printf("Directories in search path:\n");
        while (scan_token(&remaining, ":", dir, sizeof(dir))) {
            printf("%s\n", dir);
        }
    } else {
        const char slash = '/';
        for (int i = 1; argv[i] != NULL; i++) {
            //resets remaining for each input
            remaining = searchpath;
            while (scan_token(&remaining, ":", dir, sizeof(dir))) {          
                strncat(dir, &slash, 1);
                strcat(dir, argv[i]);
                //checks if both readable and executable using bitmask
                if (access(dir, R_OK | X_OK)==0) {
                    printf("%s\n", dir);
                    break;
                }
            }
        }
    }
    return 0;
}
