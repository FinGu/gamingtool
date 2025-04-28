#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "filesys.h"

void call_edit(string);

gt_error edit_game(config *cfg, string folder, string game){
    gt_error err = ok;

    string game_folder_path = str_alloc(0);
    string game_cfg_path = str_alloc(0);

    (void)cfg;

    if((err = get_game_paths(&game_folder_path, &game_cfg_path, folder, game))){
        return err;
    }

    call_edit(game_cfg_path);

    str_free(&game_folder_path);
    str_free(&game_cfg_path);

    return err;
}

gt_error edit_main(config *cfg){
    call_edit(cfg->path);
    
    return ok;
}

void call_edit(string path){
    size_t elen;
    char *editor;
    string cmd = str_alloc(0);

    if(!(editor = getenv("EDITOR"))){
        editor = "vim";
    }

    elen = strlen(editor);

    str_append_multiple(&cmd, 3, str_view(elen, editor), str_view(1, " "), path);

    (void)system(str_raw_p(&cmd));

    str_free(&cmd);
}

