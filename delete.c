#include <unistd.h>

#include "delete.h"
#include "filesys.h"

gt_error delete(config *cfg, string folder, string game){
    int st;
    size_t rsz;
    gt_error err = ok;
    string rem;
    char *tmpp;

    rsz = str_len(&folder) + str_len(&game) + 5; //5 for game/

    rem = str_alloc(rsz);

    str_append_s(&rem, folder);

    str_append_p(&rem, 5, "game/");

    str_append_s(&rem, game);

    tmpp = str_raw_p(&rem);

    err = delete_files_in_folder(tmpp);
    
    if((st = rmdir(tmpp)) == -1){
        err = failed_to_delete;
        goto out;
    }

out:
    str_free(&rem); 
    return err;
}

