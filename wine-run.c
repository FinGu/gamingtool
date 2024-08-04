#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "parse.h"
#include "errors.h"
#include "list.h"
#include "filesys.h"
#include "utils.h"
#include "run.h"

gt_error wine_run(config *cfg, string folder, string version, char *game_path){
    gt_error err = ok;
        
    if(!can_access(game_path, 0)){
        err = game_not_found;
        goto out;
    }

    game_config custom = {
        .name = "Custom wine game",
        .path = game_path,
        .wine.version = str_raw_p(&version),
    };

    custom.arguments = (struct __args){0, 1, calloc(1, sizeof(char*))};

    if((err = game_process_run(cfg, &custom, folder, NULL, cfg->debug)) && err == wine_not_found){
        printf(PREFIX"Available wine versions are listed as follows: \n");
        list(folder, "wine", true);

        goto out;
    }

out:
    free(custom.arguments.ptr);
    return err; 
}

