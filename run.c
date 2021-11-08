#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "run.h"
#include "utils.h"
#include "filesys.h"
#include "parse.h"

gt_error find_wine(string*, string, char*);

gt_error game_process_run(game_config*, string, int);

gt_error run(config* cfg, string folder, string game){
    gt_error err = ok;
    game_config gamecfg = {0};
    string game_folder = {0, NULL};

    if((err = get_game_config(&gamecfg, &game_folder, folder, game))){
        goto out;
    }
 
    err = run_game(cfg, &gamecfg, game_folder, folder);

    cfree(game_folder.ptr);

    out: 
    free_game_config(&gamecfg);

    return err;
}

gt_error find_wine(string *out, string folder, char *wine){ //we don't have the length of wine
    gt_error err = ok;
    char *cout;
    int len;

    if(!wine) {
        err = couldnt_find_wine;
        goto out;
    }

    len = folder.len + strlen(wine) + 14; //5 for wine/, 9 for /bin/wine 

    cout = smalloc(len);

    sprintf(cout, "%swine/%s/bin/wine", folder.ptr, wine);

    if(!can_access(cout, 0)){
        err = couldnt_find_wine;
        goto out;
    }

    *out = (string){len, cout};

    out:
    if(err){
        cfree(cout);
    }

    return ok;
}

gt_error run_game(config* cfg, game_config *gamecfg, string game_folder, string folder){
    gt_error err = ok, serr = ok;

    //TODO: support bash aliases?
    if(!can_access(gamecfg->folder, 1)){ 
        err = game_folder_not_found;
        goto out;
    }

    if(gamecfg->scripts.prelaunch || gamecfg->scripts.postlaunch){
        chdir(game_folder.ptr);
    }
    
    if(gamecfg->scripts.prelaunch){
        if(cfg->log){
            puts(PREFIX"Running the prelaunch script");
        }

        serr = prun("prelaunch", cfg->log);

        if(cfg->log && serr){
            puts(PREFIX"Failed to run the prelaunch script with error: ");
            print_error(serr);
        } 
    }

    if(cfg->log){
        printf(PREFIX"Running game %s\n", gamecfg->name);

        if(gamecfg->wine.enabled){
            puts(PREFIX"Wine is enabled");
        }
    }

    game_process_run(gamecfg, game_folder, cfg->log);

    if(gamecfg->scripts.postlaunch) {
        if(cfg->log){
            puts(PREFIX"Running the postlaunch script");
        }

        serr = prun("postlaunch", cfg->log);

        if(cfg->log && serr){
            puts(PREFIX"Failed to run the prelaunch script");
            print_error(serr);
        } 
    }

    out:
    return err;
}

gt_error game_process_run(game_config *gamecfg, string game_folder, int log){
    gt_error err;
    size_t len = strlen(gamecfg->executable) + 10; //10 for the full command 
    char *cmd = NULL;
    string winepath = {0, NULL};

    len += ((gamecfg->arguments) ? strlen(gamecfg->arguments) : 0);

    if(gamecfg->wine.enabled){
        if((err = find_wine(&winepath, game_folder, gamecfg->wine.version))){
            goto out;
        }
        len += winepath.len;
    }

    chdir(gamecfg->folder);

    cmd = smalloc(len);

    sprintf(cmd, "%s ./%s %s", ((gamecfg->wine.enabled) ? winepath.ptr : ""), gamecfg->executable, ((gamecfg->arguments) ? gamecfg->arguments : "")); //convenient

    err = prun(cmd, log);

    out:
    cfree(winepath.ptr);
    cfree(cmd);

    return err; 
}
