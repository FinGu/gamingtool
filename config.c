#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

#include "utils.h"
#include "filesys.h"
#include "parse.h"

gt_error get_config(config *in, string folder){
    gt_error err = ok;
    char buf[BUFSIZE] = {0};
    string cfg = {0, NULL};

    if((err = read_write_config(O_RDONLY, BUFSIZE, buf, folder))) {
        if((err = create_config(&cfg))){
            goto out;
        }

        if((err = read_write_config(O_CREAT | O_WRONLY | O_TRUNC | S_IRWXU, cfg.len, cfg.ptr, folder))){
            goto out;
        }
    }

    err = parse_config(in, ((cfg.ptr) ? cfg.ptr : buf));
    
    out:
    free(cfg.ptr);

    return err;
}

gt_error get_game_config(game_config* in, string* game_folder, string folder, string game){
    gt_error err = ok;
    char buf[BUFSIZE] = {0}; 
    string __game_folder = {0, NULL};

    if((err = get_game_folder(&__game_folder, folder, game))){
        goto out;
    }

    if((err = read_write_config(O_RDONLY, BUFSIZE, buf, __game_folder))){
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
