#include <fcntl.h>
#include <string.h>

#include "config.h"

#include "utils.h"
#include "filesys.h"
#include "parse.h"

gt_error get_config(config *in, size_t foldlen, char *folder){
    gt_error err = ok;
    size_t cfgsize = 0;
    char buf[BUFSIZE] = {0};
    char *cfgbuf = NULL;

    if((err = read_write_config(O_RDONLY, BUFSIZE, buf, foldlen, folder))) {
        if((err = create_config(&cfgsize, &cfgbuf))){
            goto out;
        }

        if((err = read_write_config(O_CREAT | O_WRONLY | O_TRUNC | S_IRWXU, cfgsize, cfgbuf, foldlen, folder))){
            goto out;
        }
    }

    err = parse_config(in, ((cfgbuf) ? cfgbuf : buf));
    
    out:
    cfree(cfgbuf);

    return err;
}

gt_error get_game_config(game_config* in, size_t *game_folder_len, char** game_folder, size_t foldlen, char *folder, size_t gamelen, char *game){
    gt_error err = ok;
    char buf[BUFSIZE] = {0}, *__game_folder = NULL;
    size_t __game_folder_len = 0;

    if((err = get_game_folder(&__game_folder_len, &__game_folder, foldlen, folder, gamelen, game))){
        goto out;
    }

    in->name = strdup(game);
    
    if((err = read_write_config(O_RDONLY, BUFSIZE, buf, __game_folder_len, __game_folder))){
        goto out;
    }

    err = parse_game_config(in, buf);

    *game_folder = __game_folder;
    
    out:
    if(err){
        cfree(__game_folder);
    }

    return err;
}
