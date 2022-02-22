#include "config.h"

gt_error create_config(string*);

gt_error create_game_config(string*, game_config);

gt_error parse_config(config*, char*); //no free function needed because the struct has no values that need to be allocated

gt_error parse_game_config(game_config*, char*);

void free_game_config(game_config*);
