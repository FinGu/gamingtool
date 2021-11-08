#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "run.h"
#include "utils.h"
#include "filesys.h"
#include "parse.h"

gt_error find_wine(size_t*, char**, size_t, char*, char*);

gt_error game_process_run(game_config*, size_t, char*, int);

gt_error run(config* cfg, size_t foldlen, char *folder, size_t gamelen, char *game){
    gt_error err = ok;
    game_config gamecfg = {0};
    char *game_folder = NULL;
    size_t game_folder_len = 0;

    if((err = get_game_config(&gamecfg, &game_folder_len, &game_folder, foldlen, folder, gamelen, game))){
        goto out;
    }
 
    err = run_game(cfg, &gamecfg, game_folder_len, game_folder, foldlen,folder);

    cfree(game_folder);

    out: 
    free_game_config(&gamecfg);

    return err;
}

gt_error find_wine(size_t *outlen, char **outstr, size_t foldlen, char *folder, char *wine){ //we don't have the length of wine
    gt_error err = ok;
    char *cout;
    int len;

    if(!wine) {
        err = couldnt_find_wine;
        goto out;
    }

    len = foldlen + strlen(wine) + 14; //5 for wine/, 9 for /bin/wine 

    cout = smalloc(len);

    sprintf(cout, "%swine/%s/bin/wine", folder, wine);

    if(!can_access(cout, 0)){
        err = couldnt_find_wine;
        goto out;
    }

    *outlen = len;
    *outstr = cout;

    out:
    if(err){
        cfree(cout);
    }

    return ok;
}

gt_error run_game(config* cfg, game_config *gamecfg, size_t game_folder_len, char *game_folder, size_t foldlen, char *folder){
    gt_error err = ok, serr = ok;
    int slen;
    char *sbuf;

    //TODO: support bash aliases?
    if(!can_access(gamecfg->folder, 1)){ 
        err = game_folder_not_found;
        goto out;
    }
    
    if(gamecfg->scripts.prelaunch){
        if(cfg->log){
            puts(PREFIX"Running the prelaunch script");
        }

        chdir(game_folder);

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

    game_process_run(gamecfg, game_folder_len, game_folder, cfg->log);

    if(gamecfg->scripts.postlaunch) {
        if(cfg->log){
            puts(PREFIX"Running the postlaunch script");
        }

        chdir(game_folder);

        serr = prun("postlaunch", cfg->log);

        if(cfg->log && serr){
            puts(PREFIX"Failed to run the prelaunch script");
            print_error(serr);
        } 
    }
    out:
    return err;
}

gt_error game_process_run(game_config *gamecfg, size_t game_folder_len, char *game_folder, int log){
    gt_error err;
    size_t wlen = 0, len = strlen(gamecfg->executable) + 8; //8 for the full command 
    char *cmd, *winepath;

    cmd = winepath = NULL;

    len += ((gamecfg->arguments) ? strlen(gamecfg->arguments) : 0);

    if(gamecfg->wine.enabled){
        if((err = find_wine(&wlen, &winepath, game_folder_len, game_folder, gamecfg->wine.version))){
            goto out;
        }
        len += wlen;
    }

    chdir(gamecfg->folder);

    cmd = smalloc(len);

    sprintf(cmd, "%s ./\"%s\" \"%s\"", ((gamecfg->wine.enabled) ? winepath : ""), gamecfg->executable, gamecfg->arguments); //convenient

    err = prun(cmd, log);

    out:
    cfree(winepath);
    cfree(cmd);

    return err; 
}
