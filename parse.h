#include "config.h"

gt_error create_config(string*);

gt_error parse_config(config*, char*); //no free function needed as this doesnt allocate to the struct

gt_error parse_game_config(game_config*, char*);

void free_game_config(game_config*);
