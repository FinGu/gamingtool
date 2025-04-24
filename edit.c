#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "filesys.h"

gt_error edit(config *cfg, string folder, string game){
    string game_folder_path = str_alloc(0);
    string game_cfg_path = str_alloc(0);

    string cmd = str_alloc(0);

    gt_error err = ok;
    size_t elen;
    char *editor;

    (void)cfg;

    if((err = get_game_paths(&game_folder_path, &game_cfg_path, folder, game))){
        return err;
    }

    if(!(editor = getenv("EDITOR"))){
        editor = "vim";
    }

    elen = strlen(editor);

    str_append_multiple(&cmd, 3, str_view(elen, editor), str_view(1, " "), game_cfg_path);

    (void)system(str_raw_p(&cmd));

    str_free(&game_folder_path);
    str_free(&game_cfg_path);
    str_free(&cmd);

    return err;

}

