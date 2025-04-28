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

    char *tmpp = NULL; 

    str_append_multiple(&location, 2, folder, str_view(6, "config"));

    fd = open(str_raw_p(&location), O_CREAT | O_RDWR, FILE_PERM);

    if(fd == -1){
        err = failed_to_open;
        goto out;
    }

    if(read(fd, buf, BUFSIZE) <= 0){
        if((err = create_config(&cfg))){
            goto out;
        } 

        tmpp = str_raw_p(&cfg);

        if(write(fd, tmpp, str_len(&cfg)) == -1){
            err = failed_to_write;
            goto out;
        }
    } else { //cfg will never be larger than BUFSIZE
        buf[BUFSIZE-1] = '\0';
    }

    err = parse_config(in, location, ((tmpp) ? tmpp : buf));
    
    out:
    str_free(&cfg);
    close(fd);

    return err;
}

gt_error get_game_paths(string *game_folder_path, string *game_config_path, string folder, string game){
    gt_error err = ok;

    string game_folder = str_alloc(0);

    size_t nlen;

    if((err = get_game_folder(&game_folder, folder, game))){
        goto out;
    }

    nlen = str_len(&game_folder);

    string path = str_alloc(nlen + 6);

    str_append_multiple(&path, 2, game_folder, str_view(6, "config"));

    *game_folder_path = game_folder;

    *game_config_path = path;

out:
    return err;
}

gt_error get_game_config(game_config* in, string* game_folder, string folder, string game){
    gt_error err = ok;
    char buf[BUFSIZE] = {0}; 
    string game_folder_path = str_alloc(0);
    string game_config_path = str_alloc(0);

    if((err = get_game_paths(&game_folder_path, &game_config_path, folder, game))){
        goto out;
    }

    if((err = read_file(BUFSIZE, buf, game_config_path))){
        goto out;
    }

    if((err = parse_game_config(in, buf))){
        goto out;
    }

    in->name = strdup(str_raw_p(&game));

    *game_folder = game_folder_path;
    
    out:
    str_free(&game_config_path);

    if(err){
        str_free(&game_folder_path);
    }

    return err;
}
