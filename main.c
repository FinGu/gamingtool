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
    char *argp;
    string folder, argh;
    config cfg = {0};

    folder = (string){0, NULL};

    if(argc < 3){
        usage();
        goto out;
    }

    if((err = get_create_folder(&folder))){
        goto out;
    } 
    
    if((err = get_config(&cfg, folder))){
        goto out;
    }

    argp = argv[1];

    argh = (string){strlen(argv[2]), argv[2]};

    if(strcmp(argp, "run") == 0) {
        err = run(&cfg, folder, argh);
    } else if(strcmp(argp, "list") == 0){
        err = ((strcmp(argh.ptr, "wine") == 0 || strcmp(argh.ptr, "game") == 0)) ?
            list(folder, argh.ptr) :
            failed_to_read;
    }

    out:
    cfree(folder.ptr);

    if(err){
        print_error(err);
    }

    return err;
}

void usage(){
    puts(PREFIX"available options:\nrun <game>\nlist <wine or games>");
}
