#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>

#include "parse.h"

#include "errors.h"
#include "filesys.h"
#include "run.h"
#include "list.h"
#include "utils.h"

void usage(void);
gt_error get_config(config*, char*);

int main(int argc, char **argv){
    gt_error err = ok;
    char *folder = NULL, *argp, *argh;
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

    argh = argv[2];

    if(strcmp(argp, "run") == 0) {
        err = run(&cfg, folder, argh);
    } else if(strcmp(argp, "list") == 0){
        err = ((strcmp(argh, "wine") == 0 || strcmp(argh, "game") == 0)) ?
            list(folder, argh) :
            failed_to_read;
    }

    out:
    free(folder);

    if(err){
        print_error(err);
    }

    return err;
}

void usage(){
    puts(PREFIX"available options:\nrun <game>\nlist <wine or games>");
}

gt_error get_config(config *out, char *folder){
    gt_error err = ok;
    int nlen;
    char buf[BUFSIZE] = {0};
    char *cfgbuf = NULL;

    if((err = read_write_config(O_RDONLY, BUFSIZE, buf, folder))) {
        if((err = create_config(&cfgbuf))){
            goto out;
        }
        
        nlen = 17; 

        if((err = read_write_config(O_CREAT | O_WRONLY | O_TRUNC | S_IRWXU, nlen, cfgbuf, folder))){
            goto out;
        }
    }

    err = parse_config(out, ((cfgbuf) ? cfgbuf : buf));
    
    out:
    free(cfgbuf);

    return err;
}
