#include <string.h>
#include <stdio.h>

#include "list.h"
#include "utils.h"
#include "filesys.h"

gt_error list(size_t foldlen, char *folder, char *arg){
    size_t fblen;
    char *folderbuf;
    gt_error err = ok;
        
    fblen = foldlen + 4; //for game or wine

    folderbuf = copycatalloc(fblen, folder, arg);

    printf(PREFIX);

    err = print_files_in_folder(folderbuf);

    cfree(folderbuf);

    return err;
}

