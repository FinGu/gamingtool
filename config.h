#ifndef GT_CFG
#define GT_CFG

#include <stddef.h>

#include "errors.h"

#define NAME "gamingtool"

#define DIRNAME "/." NAME

#define PREFIX NAME"> "

#define NSIZE 10

typedef struct {
    size_t len;
    char *ptr;
} string;

typedef struct {
    int log;
} config;

struct __game_wine{
    char *version;
};

struct __game_scripts{
    int prelaunch; //prelaunch (in the game folder)
    int postlaunch; //postlaunch (in the game folder)
};

typedef struct {
    char *name; //this var is the name of the game's folder
    char *path;
    char *arguments;
    struct __game_wine wine;
    struct __game_scripts scripts;
} game_config;

gt_error get_config(config*, string);

gt_error get_game_config(game_config*, string*, string, string);

#endif
