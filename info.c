#include <stdio.h>

#include "info.h"
#include "config.h"
#include "utils.h"
#include "parse.h"

#define AS_BOOL(x) ((x) ? "true" : "false")
#define OR_NULL(x) ((x) ? x : "(null)")

gt_error info(config *cfg, string folder, string game){
    gt_error err = ok;
    game_config gamecfg = {0};
    string game_folder = {0, NULL};

    if((err = get_game_config(&gamecfg, &game_folder, folder, game))){
        goto out;
    }

    printf(
        "%s:\n"\
        "    path: %s\n"\
        "    arguments: %s\n"\
        "    wine:\n"\
        "        version: %s\n"\
        "    scripts:\n"\
        "        prelaunch: %s\n"\
        "        postlaunch: %s\n",
        gamecfg.name, gamecfg.path, OR_NULL(gamecfg.arguments), OR_NULL(gamecfg.wine.version), AS_BOOL(gamecfg.scripts.prelaunch), AS_BOOL(gamecfg.scripts.postlaunch)
    );

    sfree(game_folder);

    free_game_config(&gamecfg);

    out:
    return err;
}
