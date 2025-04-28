#include <stdio.h>
#include <string.h>

#include <fcntl.h>

#include "filesys.h"

#include "run.h"
#include "wine-run.h"
#include "edit.h"
#include "install.h"
#include "create.h"
#include "delete.h"
#include "list.h"
#include "info.h"
#include "utils.h"
#include "parse.h"

void usage(void);

int main(int argc, char **argv){
    gt_error err = ok;
    char *argp, *tmpp;
    string folder, argh;
    config cfg = {0};

    folder = argh = str_alloc(0);

    if((err = get_create_folder(&folder))){
        goto out;
    } 
    
    if((err = get_config(&cfg, folder))){
        goto out2;
    }
    
    //create folder and config before the usage message

    argp = argv[1];

    if(argc < 3){
        if(argc == 2 && strcmp(argp, "edit") == 0){
            err = edit_main(&cfg);
            goto out2;
        }

        usage();

        goto out2;
    }

    argh = str_view(strlen(argv[2]), argv[2]);

    if(strcmp(argp, "run") == 0) {
        err = run(&cfg, folder, argh);
    } 
    else if(strcmp(argp, "info") == 0){
        err = info(&cfg, folder, argh);
    }
    else if(strcmp(argp, "list") == 0){
        tmpp = str_raw_p(&argh);

        err = ((strcmp(tmpp, "wine") == 0 || strcmp(tmpp, "game") == 0) 
                ? list(folder, tmpp, true) 
                : invalid_input);
    } 
    else if(strcmp(argp, "create") == 0){
        err = create(&cfg, folder, argh);
    }
    else if(strcmp(argp, "edit") == 0){
        err = edit_game(&cfg, folder, argh);
    }
    else if(strcmp(argp, "install") == 0){
        if(argc < 4){
            usage();

            goto out2;
        }

        err = install(&cfg, folder, argh, str_view(strlen(argv[3]), argv[3]));
    }
    else if(strcmp(argp, "wine-run") == 0 || strcmp(argp, "run-wine") == 0){
        if(argc < 4){
            usage();

            goto out2;
        }
        
        err = wine_run(&cfg, folder, argh, argv[3]);
    }
    else if(strcmp(argp, "delete") == 0){
        err = delete(&cfg, folder, argh);
    }
    else{
        usage();
    }

    out2:
    free_config(&cfg);

    out:
    str_free(&folder);

    if(err){
        print_error(err);
    }

    return err;
}

void usage(){
    puts(
        PREFIX "available options:\n" \
        "run <game> ( Runs a game )\n" \
        "wine-run <wine version> <windows exe path> ( runs an exe with the version specified )\n" \
        "info <game> ( Shows a config's structure )\n" \
        "edit <game> ( Edit a config using $EDITOR )\n" \
        "list <wine or game> ( Displays all the names of the games/wines )\n" \
        "create <game name> ( Prompts a cli tool to create a game config )\n" \
        "install <wine or game> <name> ( Installs a wine/game from the github repo )\n" \
        "delete <game> ( Removes a game )"
    );
}
