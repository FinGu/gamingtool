#ifndef GT_CFG
#define GT_CFG

#include <stddef.h>

#include "errors.h"

#define NAME "gamingtool"

#define DIRNAME "/." NAME

#define PREFIX NAME"> "

#define NSIZE 10

typedef struct {
    int log;
} config;

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
    char *folder;
    char *executable;
    char *arguments;
    struct __game_wine wine;
    struct __game_scripts scripts;
} game_config;

gt_error get_config(config*, size_t, char*);

gt_error get_game_config(game_config*, size_t*, char**, size_t, char*, size_t, char*);

#endif
