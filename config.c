#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#include "utils.h"
#include "filesys.h"
#include "parse.h"

gt_error get_config(config *in, string folder){
    int fd;
    gt_error err = ok;
    char buf[BUFSIZE] = {0};

    string location = str_alloc(folder.len + 6), //6 for config
           cfg = str_alloc(0); 

    str_append_s(&location, folder);

    str_append_p(&location, 6, "config");

    fd = open(str_raw_p(&location), O_CREAT | O_RDWR, FILE_PERM);

    if(fd == -1){
        err = failed_to_open;
        goto out;
    }

    if(read(fd, buf, BUFSIZE) <= 0){
        if((err = create_config(&cfg))){
            goto out;
        } 

        if(write(fd, cfg.ptr, cfg.len) == -1){
            err = failed_to_write;
            goto out;
        }
    } else { //cfg will never be larger than BUFSIZE
        buf[BUFSIZE-1] = '\0';
    }

    err = parse_config(in, ((cfg.ptr) ? cfg.ptr : buf));
    
    out:
    str_free(&location);
    str_free(&cfg);
    close(fd);

    return err;
}

gt_error get_game_config(game_config* in, string* game_folder, string folder, string game){
    gt_error err = ok;
    char buf[BUFSIZE] = {0}; 
    string __game_folder = str_alloc(0);

    if((err = get_game_folder(&__game_folder, folder, game))){
        goto out;
    }

    if((err = read_config(BUFSIZE, buf, __game_folder))){
        goto out;
    }

    if((err = parse_game_config(in, buf))){
        goto out;
    }

    in->name = strdup(game.ptr);

    *game_folder = __game_folder;
    
    out:
    if(err){
        free(__game_folder.ptr);
    }

    return err;
}
