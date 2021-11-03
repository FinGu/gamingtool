#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

#include "errors.h"
#include "filesys.h"
#include "run.h"
#include "list.h"
#include "utils.h"

void usage(){
    puts(PREFIX"available options:\nrun <game>\nlist <wine or games>");
}

int main(int argc, char **argv){
    gt_error err = 0;
    char *folder, *folderbuf, *argp, *argh;

    folder = folderbuf = NULL;

    if(argc < 3){
        usage();
        goto out;
    }

    if((err = get_create_folder(&folder))){
        goto out;
    } 

    argp = argv[1];

    argh = argv[2];

    if(strcmp(argp, "run") == 0) {
        err = run(folder, argh);
    } else if(strcmp(argp, "list") == 0){
        err = ((strcmp(argh, "wine") == 0 || strcmp(argh, "game") == 0)) ?
            list(folder, argh) :
            failed_to_read;
    }

    out:
    free(folderbuf);
    free(folder);

    if(err){
        print_error(err);
    }

    return err;
}
