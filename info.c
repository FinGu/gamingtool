#include <stdio.h>

#include "info.h"
#include "config.h"
#include "parse.h"

#define AS_BOOL(x) ((x) ? "true" : "false")
#define OR_NULL(x) ((x) ? x : "(null)")

gt_error info(config *cfg, string folder, string game){
    int i, sz;
    gt_error err = ok;
    game_config gamecfg = {0};
    string game_folder = str_alloc(0);

    if((err = get_game_config(&gamecfg, &game_folder, folder, game))){
        goto out;
    }

    printf(
        "%s:\n"\
        "    path: %s\n"\
        "    arguments: ", gamecfg.name, gamecfg.path
    );

    i = gamecfg.wine.version != NULL;

    sz = gamecfg.arguments.size;

    if(sz && !(sz == 1 && i)){
        for(; i < sz; ++i){
            printf( ((i == sz-1) ? "%s\n" : "%s, "), OR_NULL(gamecfg.arguments.ptr[i]));
        }
    } else {
        puts(OR_NULL(NULL));
    }

    printf(
        "    wine:\n" \
        "        version: %s\n"\
        "    scripts:\n"\
        "        prelaunch: %s\n"\
        "        postlaunch: %s\n",
        OR_NULL(gamecfg.wine.version), AS_BOOL(gamecfg.scripts.prelaunch), AS_BOOL(gamecfg.scripts.postlaunch)
    );

    str_free(&game_folder);

    free_game_config(&gamecfg);

    out:
    return err;
}
