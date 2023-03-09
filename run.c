#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <sys/stat.h>

#include "run.h"
#include "utils.h"
#include "filesys.h"
#include "parse.h"

gt_error find_wine(string*, string, char*);

gt_error game_process_run(game_config*, string, char*, bool);

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

gt_error find_wine(string *out, string folder, char *wine){ //we don't have the length of wine
    gt_error err = ok;
    string cout;
    size_t wlen, len;
    char *tmpp;

    if(!wine) {
        err = couldnt_find_wine;
        goto out;
    }

    wlen = strlen(wine);

    len = str_len(&folder) + wlen + 14; //5 for wine/, 9 for /bin/wine

    cout = str_alloc(len);

    str_append_s(&cout, folder);

    str_append_p(&cout, 5, "wine/");

    str_append_p(&cout, wlen, wine);

    str_append_p(&cout, 1, "/");

    tmpp = str_raw_p(&cout);

    // "%swine/%s/"

    if(!can_access(tmpp, S_IFDIR)){
        err = couldnt_find_wine;
        goto out;
    }

    str_append_p(&cout, 4, "wine");

    if(can_access(tmpp, S_IXUSR)){
        goto out;
    } 

    str_clear(&cout, 4);
    
    //memset(&cout.ptr[len-8], 0, 4); //clears only wine

    str_append_p(&cout, 8, "bin/wine");

    if (can_access(tmpp, S_IXUSR)){
        goto out;
    }

    err = couldnt_find_wine;

    out:
    if(err){
        str_free(&cout);
    } else{
        *out = cout;
    }

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

        str_append_s(&scpath, game_folder);
    }
    
    if(gamecfg->scripts.prelaunch){
        if(cfg->debug){
            puts(PREFIX"Running the prelaunch script");
        }

        str_append_p(&scpath, 9, "prelaunch");
        
        tmpp = str_raw_p(&scpath);

        serr = (can_access(tmpp, S_IXUSR) ? prun(tmpp, NULL, &gamecfg->environment, NULL, cfg->debug) : failed_to_execute); 

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

        str_append_s(&logpath, folder);

        str_append_p(&logpath, 4, "log/");

        str_append_p(&logpath, strlen(gamecfg->name), gamecfg->name);
        
        str_append_p(&logpath, 1, "/");

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

            str_append_p(&logpath, 30, namebuf);
        }
    }

    if(cfg->debug){
        if(gamecfg->wine.version){
            puts(PREFIX"Wine is enabled");
        }

        printf(PREFIX"Running game %s\n", gamecfg->name);
    }

    tmpp = str_raw_p(&logpath);

    err = game_process_run(gamecfg, folder, tmpp, cfg->debug);

    if(gamecfg->scripts.postlaunch) {
        if(cfg->debug){
            puts(PREFIX"Running the postlaunch script");
        }

        str_append_p(&scpath, 10, "postlaunch");
        
        tmpp = str_raw_p(&scpath);

        serr = (can_access(tmpp, S_IXUSR) ? prun(tmpp, NULL, &gamecfg->environment, NULL, cfg->debug) : failed_to_execute);

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

gt_error game_process_run(game_config *gamecfg, string folder, char *log_path, bool log_to_stdout){
    gt_error err = ok;

    size_t proglen, pathlen = strlen(gamecfg->path), tidx; //4 for the full command, 1 for 0

    char *tmpp;
    string program, executable, winepath = str_alloc(0);

    struct __args *arguments = &gamecfg->arguments, *environment = &gamecfg->environment;

    executable = get_file_from_path(str_view(pathlen, gamecfg->path));

    if(gamecfg->wine.version && (err = find_wine(&winepath, folder, gamecfg->wine.version))){
        goto out;
    }
    
    tidx = pathlen - str_len(&executable) - 1;

    gamecfg->path[tidx] = '\0'; // sets the last slash to a 0 to get the folder path without the executable

    chdir(gamecfg->path);

    gamecfg->path[tidx] = '/'; // sets the original value back, for debugging purposes

    proglen = str_len(&executable) + 2; // size of first str = 2

    program = str_alloc(proglen);
    
    str_append_p(&program, 2, "./");

    str_append_s(&program, executable);
    
    tmpp = str_raw_p(&program);

    if(gamecfg->wine.version){
        arguments->ptr[0] = tmpp; // pass program as an arg to wine

        err = prun(str_raw_p(&winepath), arguments, environment, log_path, log_to_stdout); // run wine
    } else {
        err = prun(tmpp, arguments, environment, log_path, log_to_stdout);
    }

    str_free(&winepath);
    str_free(&program);

    out:
    return err; 
}
