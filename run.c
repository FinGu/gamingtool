#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
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
    string game_folder = {0, NULL};

    if((err = get_game_config(&gamecfg, &game_folder, folder, game))){
        goto out;
    }
 
    err = run_game(cfg, &gamecfg, game_folder, folder);

    sfree(game_folder);

    free_game_config(&gamecfg);

    out: 
    return err;
}

gt_error find_wine(string *out, string folder, char *wine){ //we don't have the length of wine
    gt_error err = ok;
    string cout;
    size_t len;

    if(!wine) {
        err = couldnt_find_wine;
        goto out;
    }

    len = folder.len + strlen(wine) + 14; //5 for wine/, 9 for /bin/wine, 1 for 0

    cout = salloc(len);

    sprintf(cout.ptr, "%swine/%s/", folder.ptr, wine);

    if(!can_access(cout.ptr, 0)){
        err = couldnt_find_wine;
        goto out;
    }

    pstrcat(cout.ptr, "wine");

    if(can_access(cout.ptr, 0)){
        goto out;
    } 
    
    memset(&cout.ptr[len-8], 0, 4); //clears only wine

    pstrcat(cout.ptr, "bin/wine");

    if (can_access(cout.ptr, 0)){
        goto out;
    }

    err = couldnt_find_wine;

    out:
    if(err){
        sfree(cout);
    } else{
        *out = cout;
    }

    return err;
}

gt_error run_game(config* cfg, game_config *gamecfg, string game_folder, string folder){
    gt_error err = ok, serr = ok;
    char *scpath, *logpath, namebuf[40];

    time_t timenow;
    struct tm* timeinfo;

    scpath = logpath = NULL;

    //TODO: support bash aliases?
    if(!can_access(gamecfg->path, 0)){ 
        err = game_not_found;
        goto out;
    }

    if(gamecfg->scripts.prelaunch || gamecfg->scripts.postlaunch){
        scpath = scalloc(game_folder.len + 11, sizeof(char)); //10 for the script name, 1 for 0

        pstrcpy(scpath, game_folder.ptr);
    }
    
    if(gamecfg->scripts.prelaunch){
        if(cfg->debug){
            puts(PREFIX"Running the prelaunch script");
        }

        pstrcat(scpath, "prelaunch");

        serr = (can_access(scpath, 0) ? prun(scpath, NULL, cfg->debug) : failed_to_execute); 

        memset(&scpath[game_folder.len], 0, 9); // clears prelaunch

        if(cfg->debug && serr){
            puts(PREFIX"Failed to run the prelaunch script with error: ");
            print_error(serr);
        } 
    }

    if(cfg->log){ //only game specific output should be logged
        if(cfg->debug){
            puts(PREFIX"Logging is enabled");
        }

        logpath = scalloc(game_folder.len + 31, sizeof(char)); //30 for the name of the file, 1 for the term

        sprintf(logpath, "%slog/%s/", folder.ptr, gamecfg->name);

        serr = (!can_access(logpath, S_IFDIR) && !__mkdir(logpath)) ? failed_to_create_dir : ok;

        timenow = time(NULL);

        timeinfo = localtime(&timenow);

        strftime(namebuf, 30, "%Y-%m-%d %H:%M:%S.txt", timeinfo);

        pstrcat(logpath, namebuf);

        if(serr){
            if(cfg->debug){
                puts(PREFIX"Error while making the log folder");
                print_error(serr);
            }
            free(logpath);
            logpath = NULL;
        }
    }

    if(cfg->debug){
        if(gamecfg->wine.version){
            puts(PREFIX"Wine is enabled");
        }

        printf(PREFIX"Running game %s\n", gamecfg->name);
    }

    err = game_process_run(gamecfg, folder, logpath, cfg->debug);

    if(gamecfg->scripts.postlaunch) {
        if(cfg->debug){
            puts(PREFIX"Running the postlaunch script");
        }

        pstrcat(scpath, "postlaunch");

        serr = (can_access(scpath, 0) ? prun(scpath, NULL, cfg->debug) : failed_to_execute);

        //no need to clear postlaunch

        if(cfg->debug && serr){
            puts(PREFIX"Failed to run the prelaunch script");
            print_error(serr);
        } 
    }

    out:
    free(scpath);
    free(logpath);

    return err;
}

gt_error game_process_run(game_config *gamecfg, string folder, char *log_path, bool log_to_stdout){
    gt_error err = ok;

    size_t clen = 10 + 1, plen = strlen(gamecfg->path), tidx; //10 for the full command, 1 for 0
    char *cmd = NULL;

    string executable, arguments;
    string winepath, a, b, c;

    winepath = a = b = c = (string){0, NULL};

    executable = get_file_from_path((string){plen, gamecfg->path});

    arguments = ((gamecfg->arguments) ? (string){strlen(gamecfg->arguments), gamecfg->arguments} : c);

    if(gamecfg->wine.version && (err = find_wine(&winepath, folder, gamecfg->wine.version))){
        goto out;
    }
    
    tidx = plen - executable.len - 1;

    gamecfg->path[tidx] = '\0'; // sets the last slash to a 0 to get the folder path without the executable

    chdir(gamecfg->path);
    
    gamecfg->path[tidx] = '/'; // sets the original value back, for debugging purposes

    escapeshellargs(&a, executable);

    escapeshellargs(&b, arguments);

    clen += a.len + b.len;
    
    if(gamecfg->wine.version){
        escapeshellargs(&c, winepath); 

        cmd = scalloc(clen + c.len, sizeof(char));

        sprintf(cmd, "'%s' ./'%s' '%s'", c.ptr, a.ptr, b.ptr); //convenient
    } else {
        cmd = scalloc(clen, sizeof(char));

        sprintf(cmd, "./'%s' '%s'", a.ptr, b.ptr);
    }

    err = prun(cmd, log_path, log_to_stdout);

    sfree(a);
    sfree(b);
    sfree(c);

    sfree(winepath);
    free(cmd);

    out:
    return err; 
}
