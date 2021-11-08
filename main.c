#include <stdio.h>
#include <string.h>

#include <fcntl.h>

#include "parse.h"

#include "config.h"
#include "filesys.h"
#include "run.h"
#include "list.h"
#include "utils.h"

void usage(void);

int main(int argc, char **argv){
    gt_error err = ok;
    char *folder = NULL, *argp, *argh;
    config cfg = {0};
    size_t foldlen = 0; 

    if(argc < 3){
        usage();
        goto out;
    }

    if((err = get_create_folder(&foldlen, &folder))){
        goto out;
    } 
    
    if((err = get_config(&cfg, foldlen, folder))){
        goto out;
    }

    argp = argv[1];

    argh = argv[2];

    if(strcmp(argp, "run") == 0) {
        err = run(&cfg, foldlen, folder, strlen(argh), argh);
    } else if(strcmp(argp, "list") == 0){
        err = ((strcmp(argh, "wine") == 0 || strcmp(argh, "game") == 0)) ?
            list(foldlen, folder, argh) :
            failed_to_read;
    }

    out:
    cfree(folder);

    if(err){
        print_error(err);
    }

    return err;
}

void usage(){
    puts(PREFIX"available options:\nrun <game>\nlist <wine or games>");
}
