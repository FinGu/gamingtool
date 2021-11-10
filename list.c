#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "utils.h"
#include "filesys.h"

gt_error list(string folder, char *arg){
    size_t fblen;
    char *folderbuf;
    gt_error err = ok;
        
    fblen = folder.len + 4; //for game or wine

    folderbuf = copycatalloc(fblen, folder.ptr, arg);

    printf(PREFIX);

    err = print_files_in_folder(folderbuf);

    free(folderbuf);

    return err;
}

