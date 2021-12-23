#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>

#include "filesys.h"

#include "run.h"
#include "list.h"
#include "info.h"

void usage(void);

int main(int argc, char **argv){
    gt_error err = ok;
    char *argp;
    string folder = {0, NULL}, argh;
    config cfg = {0};

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
    } 
    else if(strcmp(argp, "info") == 0){
        err = info(&cfg, folder, argh);
    }
    else if(strcmp(argp, "list") == 0){
        err = ((strcmp(argh.ptr, "wine") == 0 || strcmp(argh.ptr, "game") == 0) 
                ? list(folder, argh.ptr) 
                : invalid_input);
    }

    out:
    free(folder.ptr);

    if(err){
        print_error(err);
    }

    return err;
}

void usage(){
    puts(PREFIX"available options:\nrun <game>\ninfo <game>\nlist <wine or game>");
}
