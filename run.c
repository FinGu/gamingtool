#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "run.h"
#include "utils.h"
#include "filesys.h"
#include "config.h"

gt_error run(char *folder, char *game){
    gt_error err = ok;
    game_config gamecfg = {0};
    char buf[BUFSIZE] = {0}, *game_folder;

    if((err = get_game_folder(&game_folder, folder, game))){
        return err;
    }

    gamecfg.name = strdup(game);
    
    if((err = read_game(BUFSIZE, buf, game_folder))){
        goto out;
    }

    if((err = parse_game(&gamecfg, buf))){
        goto out;
    }

    err = run_game(&gamecfg, game_folder, folder);

    out: 
    free(game_folder);
    free_game(&gamecfg);

    return err;
}

gt_error find_wine(char **out, char *folder, char *wine){
    char *cout;
    int len;

    if(!wine) {
        return couldnt_find_wine;
    }

    len = strlen(folder) + strlen(wine) + 14; //5 for wine/, 9 for /bin/wine

    cout = smalloc(len);

    sprintf(cout, "%swine/%s/bin/wine", folder, wine);

    if(!can_access(cout, 0)){
        free(cout);
        return couldnt_find_wine;
    }

    *out = cout;

    return ok;
}

gt_error run_game(game_config *cfg, char *game_folder, char *folder){
    gt_error err = ok, serr = ok;
    int slen, wlen, glen;
    char *gbuf, *wbuf, *sbuf, *winepath;

    //TODO: support bash aliases?
    if(!cfg->executable || !can_access(cfg->executable, 0)){ 
        err = game_not_found;
        return err;
    }
    
    glen = strlen(cfg->executable) + ((cfg->arguments) ? strlen(cfg->arguments) : 0) + 1; //1 for " "

    gbuf = copycatalloc(glen, cfg->executable, " ");

    if(cfg->arguments){
        pstrcat(gbuf, cfg->arguments);
    }

    if(cfg->scripts.prelaunch){
        if(cfg->log){
            puts(PREFIX"Running the prelaunch script");
        }

        slen = strlen(game_folder) + 9; //9 for prelaunch

        sbuf = copycatalloc(slen, game_folder, "prelaunch");

        serr = ((can_access(sbuf, 0)) ? prun(sbuf, cfg->log) : failed_to_start);

        if(cfg->log && serr){
            puts(PREFIX"Failed to run the prelaunch script with error: ");
            print_error(serr);
        } 

        free(sbuf);
    }

    printf(PREFIX"Running game %s\n", cfg->name);

    if(cfg->wine.enabled){
        if(cfg->log){
            puts(PREFIX"Wine is enabled");
        }

        if((err = find_wine(&winepath, folder, cfg->wine.version))){
            free(gbuf);
            return err;
        }

        wlen = strlen(winepath) + 1 + glen;

        wbuf = copycatalloc(wlen, winepath, " "); //1 for " " 

        pstrcat(wbuf, gbuf);
        
        err = prun(wbuf, cfg->log);

        free(winepath);

        free(wbuf);
    } else {
        err = prun(gbuf, cfg->log);
    }
    
    free(gbuf);

    if(cfg->scripts.postlaunch) {
        if(cfg->log){
            puts(PREFIX"Running the postlaunch script");
        }

        slen = strlen(game_folder) + 10; //10 for postlaunch

        sbuf = copycatalloc(slen, game_folder, "postlaunch");

        serr = ((can_access(sbuf, 0)) ? prun(sbuf, cfg->log) : failed_to_start);

        if(cfg->log && serr){
            puts(PREFIX"Failed to run the prelaunch script");
            print_error(serr);
        } 

        free(sbuf);
    }

    return err;
}
