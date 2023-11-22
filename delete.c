#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <ctype.h>
#include <string.h>

#include "delete.h"
#include "filesys.h"
#include "parse.h"

#include "utils.h"

gt_error delete(config *cfg, string folder, string game){
    int tmpi;
    size_t rsz;
    gt_error err = ok;
    string rem;
    char *tmpp;

    rsz = str_len(&folder) + str_len(&game) + 5; //5 for game/

    rem = str_alloc(rsz);

    str_append_multiple(&rem, 3, folder, str_view(5, "game/"), game);

    tmpp = str_raw_p(&rem);

    if(!can_access(tmpp, S_IFDIR)){
        err = failed_to_delete;
        goto out;
    }

    printf("Delete game folder ( Y/n ): ");

    if(toupper(getchar()) == 'Y'){
        game_config gamecfg = {0};
        string executable, game_folder = str_alloc(0);

        if((err = get_game_config(&gamecfg, &game_folder, folder, game))){
            goto free;
        } 
        
        if(!can_access(gamecfg.path, S_IWUSR)){
            goto free;
        }

        int pathlen = strlen(gamecfg.path);

        executable = get_file_from_path(str_view(pathlen, gamecfg.path));

        tmpi = pathlen - str_len(&executable) - 1;

        gamecfg.path[tmpi] = '\0';

        printf("Are you sure you want to delete the folder %s ? ( Y/n ): ", gamecfg.path);

        stdin->_IO_read_ptr = stdin->_IO_read_end; 

        if(toupper(getchar()) != 'Y'){ // make sure a second time!
            goto free;
        }

        if((err = delete_files_in_folder(gamecfg.path))){
            print_error(err);
        } else{
            rmdir(gamecfg.path);
        }

        free:
            free_game_config(&gamecfg);
            str_free(&game_folder);
    }

    err = delete_files_in_folder(tmpp);
    
    if(rmdir(tmpp) == -1){
        err = failed_to_delete;
        goto out;
    } 
    
out:
    str_free(&rem); 
    return err;
}

