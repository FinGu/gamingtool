#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "filesys.h"

gt_error list(string folder, char *arg){
    size_t fblen;
    string folderbuf;
    gt_error err = ok;
        
    fblen = str_len(&folder) + 4; //for game or wine

    folderbuf = str_alloc(fblen);

    str_append_s(&folderbuf, folder);

    str_append_p(&folderbuf, strlen(arg), arg);

    fputs(PREFIX, stdout);

    err = print_files_in_folder(str_raw_p(&folderbuf));

    str_free(&folderbuf);

    return err; 
}

