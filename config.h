#ifndef GT_CFG
#define GT_CFG

#include <stddef.h>
#include <stdbool.h>

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
    bool log; //log game output into files
    bool debug; //log debugging info and output of scripts to stdout
} config;

struct __args {
    int size;
    char **ptr;
};

struct __game_wine{
    char *version;
};

struct __game_scripts{
    bool prelaunch; //prelaunch (in the game folder)
    bool postlaunch; //postlaunch (in the game folder)
};

typedef struct {
    char *name; //this var is the name of the game's folder
    char *path;
    struct __args arguments;
    struct __game_wine wine;
    struct __game_scripts scripts;
} game_config;

gt_error get_config(config*, string);

gt_error get_game_config(game_config*, string*, string, string);

#endif
