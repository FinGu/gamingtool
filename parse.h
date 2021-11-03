#include "errors.h"

struct __game_wine{
    int enabled;
    char *version;
};

struct __game_scripts{
    int prelaunch; //prelaunch (in the game folder)
    int postlaunch; //postlaunch (in the game folder)
};

typedef struct {
    char *name; //this var is the name of the game's folder
    char *executable;
    char *arguments;
    struct __game_wine wine;
    struct __game_scripts scripts;
    int log; //should be a global config
} game_config;

gt_error parse_game(game_config*, char*);

void free_game(game_config*);
