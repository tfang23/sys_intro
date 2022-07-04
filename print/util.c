/* File: util.c
 * Tiantian Fang CS107
 * -------------------
 * This program contains 2 functions get_env_value and scan_token that are used in the myprintenv.c program. 
 * Description for each function is written below.
 */
#include "samples/prototypes.h"
#include <string.h>

/* FUNCTION: get_env_value
 * -----------------------
 * Parameter:
 *     envp - an array of strings of the form "VARNAME=VALUE'
 *     key - string to match with VARNAME
 * Returns:
 *     value of the variable name of interest
 *
 * This function iterates through the entries in the environment variables array and looks for a matching entry.
 */
const char *get_env_value(const char *envp[], const char *key) {
    for (int i = 0; envp[i] != NULL; i++) {
         char *substr = strstr(envp[i], key);
         //checks if the substring is found and makes sure that it's not just the prefix
         if (substr != NULL && *(substr+strlen(key)) == '=') {
            return strchr(envp[i], '=') + 1;
       }
    }
    return NULL;
}

/* FUNCTION: scan_token
 * --------------------
 * Parameter:
 *     p_input - the input string
 *     delimeters - a string of delimeters
 *     buf - a fixed-length array to store the token
 *     buflen - length of the buffer
 * Returns:
 *     whether a token was written to buf
 *
 * This function separates the input into tokens in the same way that strtok does by scanning the input string to determine the extent of the next token, using the delimiters as separators, and then writing the token characters to buf.
 */
bool scan_token(const char **p_input, const char *delimiters, char buf[], size_t buflen) {
    //skips chuncks of delimiters
    *p_input += strspn(*p_input, delimiters);
    if (**p_input != '\0') {
        int span = strcspn(*p_input, delimiters);
        //uses ternary operator
        int leng = (span >= buflen)?(buflen-1):span;
        strncpy(buf, *p_input, leng);
        buf[leng] = '\0';
        *p_input += leng;
        return true;
    }
    return false;
}
