#include "config.h"
#include "errors.h"

gt_error create_config(char **);

gt_error parse_config(config*, char*);

gt_error parse_game_config(game_config*, char*);

void free_game_config(game_config*);
