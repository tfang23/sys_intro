/* FILE: myls.c
 * Tiantian Fang CS107
 *
 * This file contains my version of ls, which lists the directory entries from a path.
 */
#include <dirent.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <error.h>

enum { SORT_BY_NAME, SORT_BY_TYPE };
enum { EXCLUDE_DOT, INCLUDE_DOT };

/* On the myth filesystem, the only file type information that is accurate is
 * directory/not-directory used here. Other type info in struct dirent is 
 * not reliable.
 */

/* FUNCTION: is_dir
 *
 * Parameters:
 *    dirptr - pointer to a path
 * Returns: whether this is a directory
 *
 * This function checks if the path is a directory.
 */
bool is_dir(const struct dirent *dirptr) {
    return dirptr->d_type == DT_DIR;
}

/* FUNCTION: alpha_compare
 * 
 * Parameters:
 *     ptr1 - the first path
 *     ptr2 - the second path
 * Returns: how the two paths compare lexicographically
 *
 * This function gives the lexicographical order of the two paths.
 */
int alpha_compare(const struct dirent **ptr1, const struct dirent **ptr2) {
    return strcmp((*ptr1)->d_name, (*ptr2)->d_name);
}

/* FUNCTION: type_compare
 *
 * Parameters:
 *     ptr1 - the first path
 *     ptr2 - the second path
 * Returns: how the two paths compare in terms of type
 *
 * This function gives the order of the two paths in terms of type. If they are of the same type, then give the lexicographical order.  
 */
int type_compare(const struct dirent **ptr1, const struct dirent **ptr2) {
    bool dir1 = is_dir(*ptr1);
    bool dir2 = is_dir(*ptr2);
    if (dir1 == true && dir2 == false) {
        return -1;
    } else if (dir1 == false && dir2 == true) {
        return 1;
    }
    return alpha_compare(ptr1, ptr2);
}

/* FUNCTION: exclude_filter
 *
 * Parameters:
 *     ptr1 - pointer to a path
 * Returns: whether the path starts with a dot
 *
 * This function filters out the paths that start with a dot.
 */
int exclude_filter(const struct dirent *ptr1) {
    if(ptr1->d_name[0] != '.') {
        return 1;
    }
    return 0;
}

/* FUNCTION: ls
 *
 * Parameters:
 *    dirpath - directory path
 *    filter - inputted filter
 *    order - requested order of entries in the list
 * Returns: NA
 *
 * This function lists the directory entries in the path according to the given filter and order.
 */
void ls(const char *dirpath, int filter, int order) {
    struct dirent **namelist;
    int (*compar)(const struct dirent **, const struct dirent **);
    int (*filt)(const struct dirent *);
    if (order == SORT_BY_NAME) {
        compar = alpha_compare;
    } else {
        compar = type_compare;
    }
    if (filter == EXCLUDE_DOT) {
        filt = exclude_filter;
    } else {
        //include dot means print everything
        filt = NULL;
    }    
    int n = scandir(dirpath, &namelist, filt, compar);
    if (n == -1) {
        error(0, 0, "cannot access %s\n", dirpath);
        
    } else {
        for (int i = 0; i < n; i ++) {
            //print with a slash if the item is a directory
            if (is_dir(namelist[i])) {
                    printf("%s/\n", namelist[i]->d_name);
            } else {
                    printf("%s\n", namelist[i]->d_name);
                }
            free(namelist[i]);
        }
        free(namelist);
    }
}

int main(int argc, char *argv[]) {
    int order = SORT_BY_NAME;
    int filter = EXCLUDE_DOT;
    //checks which flags are included
    int opt = getopt(argc, argv, "az");
    while (opt != -1) {
        if (opt == 'a') {
            filter = INCLUDE_DOT;
        } else if (opt == 'z') {
            order = SORT_BY_TYPE;
        } else {
            return 1;
        }

        opt = getopt(argc, argv, "az");
    }
    //prints list of directory for each given path. If no path is given, print list of directory for the current path.
    if (optind < argc - 1) {
        for (int i = optind; i < argc; i++) {
            printf("%s:\n", argv[i]);
            ls(argv[i], filter, order);
            printf("\n");
        }
    } else {
        ls(optind == argc - 1 ? argv[optind] : ".", filter, order);
    }
    
    return 0;
}
