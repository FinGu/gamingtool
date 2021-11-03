#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include "utils.h"
#include "filesys.h"
#include "config.h"

gt_error list(char *folder, char *arg){
    int fblen;
    char *folderbuf;
    gt_error err = ok;
        
    fblen = strlen(folder) + 4; //for game or wine

    folderbuf = copycatalloc(fblen, folder, arg);

    printf(PREFIX);

    err = print_files_in_folder(folderbuf);

    free(folderbuf);

    return err;
}

