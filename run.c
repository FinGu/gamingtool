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

    game_process_run(gamecfg, folder, cfg->log);

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

gt_error game_process_run(game_config *gamecfg, string folder, int log){
    gt_error err;

    size_t len = 10; //10 for the full command 

    string executable, arguments;
    string winepath, a, b, c;
    char *cmd = NULL;

    winepath = a = b = c = (string){0, NULL};

    executable = (string){strlen(gamecfg->executable), gamecfg->executable};

    arguments = ((gamecfg->arguments) ? (string){strlen(gamecfg->arguments), gamecfg->arguments} : c);

    if(gamecfg->wine.enabled && (err = find_wine(&winepath, folder, gamecfg->wine.version))){
        goto out;
    }

    chdir(gamecfg->folder);

    escapeshellargs(&a, executable);

    escapeshellargs(&b, arguments);

    len += a.len + b.len;
    
    if(gamecfg->wine.enabled){
        escapeshellargs(&c, winepath); 

        cmd = smalloc(len + c.len);

        sprintf(cmd, "'%s' ./'%s' '%s'", c.ptr, a.ptr, b.ptr); //convenient
    } else {
        cmd = smalloc(len);

        sprintf(cmd, "./'%s' '%s'", a.ptr, b.ptr);
    }

    err = prun(cmd, log);

    cfree(a.ptr);
    cfree(b.ptr);
    cfree(c.ptr);

    cfree(winepath.ptr);
    cfree(cmd);

    out:
    return err; 
}
