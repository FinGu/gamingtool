#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <sys/stat.h>

#include "run.h"
#include "errors.h"
#include "utils.h"
#include "filesys.h"
#include "parse.h" 

gt_error run(config* cfg, string folder, string game){
    gt_error err = ok;
    game_config gamecfg = {0};
    string game_folder = str_alloc(0);

    if((err = get_game_config(&gamecfg, &game_folder, folder, game))){
        goto out;
    } 
 
    err = run_game(cfg, &gamecfg, game_folder, folder);

    str_free(&game_folder);

    free_game_config(&gamecfg);

    out: 
    return err;
}

gt_error run_game(config* cfg, game_config *gamecfg, string game_folder, string folder){
    gt_error err = ok, serr = ok;
    string scpath, logpath;
    char *tmpp, namebuf[40];

    time_t timenow;
    struct tm* timeinfo;

    scpath = logpath = str_alloc(0);

    //TODO: support bash aliases?
    if(!can_access(gamecfg->path, 0)){ 
        err = game_not_found;
        goto out;
    }

    if(gamecfg->scripts.prelaunch || gamecfg->scripts.postlaunch){
        scpath = str_alloc(str_len(&game_folder) + 10); //10 for the script name 

        str_append(&scpath, game_folder);
    }
    
    if(gamecfg->scripts.prelaunch){
        if(cfg->debug){
            puts(PREFIX"Running the prelaunch script");
        }

        str_append(&scpath, str_view(9, "prelaunch"));
        
        tmpp = str_raw_p(&scpath);

        serr = (can_access(tmpp, S_IXUSR) ? prun(tmpp, NULL, &cfg->environment, &gamecfg->environment, NULL, cfg->debug) : failed_to_execute); 

        str_clear(&scpath, 9); //clears prelaunch

        if(cfg->debug && serr){
            puts(PREFIX"Failed to run the prelaunch script with error: ");
            print_error(serr);
        } 
    }

    if(cfg->log){ //only game specific output should be logged
        if(cfg->debug){
            puts(PREFIX"Logging is enabled");
        }

        logpath = str_alloc(str_len(&game_folder)-1 + 30); //30 for the name of the file
        // len(game/xx/) == len(log/xx/) + 1

        str_append_multiple(&logpath, 4, folder, str_view(4, "log/"), str_view(strlen(gamecfg->name), gamecfg->name), str_view(1, "/"));

        tmpp = str_raw_p(&logpath);

        serr = (!can_access(tmpp, S_IFDIR) && !__mkdir(tmpp)) ? failed_to_create_dir : ok;

        if(serr){
            if(cfg->debug){
                puts(PREFIX"Error while making the log folder");
                print_error(serr);
            }

            //get_file_from_path()
            //len - ffplen - 1
            //__mkdir();

            str_free(&logpath);
        } else{
            timenow = time(NULL);

            timeinfo = localtime(&timenow);

            strftime(namebuf, 30, "%Y-%m-%d %H:%M:%S.txt", timeinfo);

            str_append(&logpath, str_view(30, namebuf));
        }
    }

    if(cfg->debug){
        if(gamecfg->wine.version){
            puts(PREFIX"Wine is enabled");
        }

        printf(PREFIX"Running game %s\n", gamecfg->name);
    }

    tmpp = str_raw_p(&logpath);

    err = game_process_run(cfg, gamecfg, folder, tmpp, cfg->debug);

    if(gamecfg->scripts.postlaunch) {
        if(cfg->debug){
            puts(PREFIX"Running the postlaunch script");
        }

        str_append(&scpath, str_view(10, "postlaunch"));
        
        tmpp = str_raw_p(&scpath);

        serr = (can_access(tmpp, S_IXUSR) ? prun(tmpp, NULL, &cfg->environment, &gamecfg->environment, NULL, cfg->debug) : failed_to_execute);

        //no need to clear postlaunch

        if(cfg->debug && serr){
            puts(PREFIX"Failed to run the prelaunch script");
            print_error(serr);
        } 
    }

    out:
    str_free(&scpath);
    str_free(&logpath);

    return err;
}

gt_error game_process_run(config *cfg, game_config *gamecfg, string folder, char *log_path, bool log_to_stdout){
    gt_error err = ok;

    size_t proglen, pathlen = strlen(gamecfg->path), tidx; //4 for the full command, 1 for 0

    char *tmpp;
    string program, executable, winepath = str_alloc(0);

    struct __args *arguments = &gamecfg->arguments, *game_environment = &gamecfg->environment, *cfg_environment = &cfg->environment;

    executable = get_file_from_path(str_view(pathlen, gamecfg->path)); 

    if(gamecfg->wine.version && (err = find_wine(&winepath, folder, gamecfg->wine.version))){
        err = wine_not_found;
        goto out;
    }
    
    tidx = pathlen - str_len(&executable) - 1;

    gamecfg->path[tidx] = '\0'; // sets the last slash to a 0 to get the folder path without the executable

    (void)chdir(gamecfg->path);

    gamecfg->path[tidx] = '/'; // sets the original value back, for debugging purposes

    proglen = str_len(&executable) + 2; // size of first str = 2

    program = str_alloc(proglen);
    
    str_append_multiple(&program, 2, str_view(2, "./"), executable);
    
    tmpp = str_raw_p(&program);

    if(gamecfg->wine.version){
        arguments->ptr[0] = tmpp; // pass program as an arg to wine

        err = prun(str_raw_p(&winepath), arguments, cfg_environment, game_environment, log_path, log_to_stdout); // run wine
    } else {
        err = prun(tmpp, arguments, cfg_environment, game_environment, log_path, log_to_stdout);
    }

    str_free(&winepath);
    str_free(&program);

    out:
    return err; 
}
