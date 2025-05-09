#ifndef GT_CFG
#define GT_CFG

#include "str.h"

#include "errors.h"

#define NAME "gamingtool"

#define NSIZE 10

#define DIRNAME "/." NAME

#define LIBURL "https://media.githubusercontent.com/media/FinGu/gamingtool-library/main/"

#define LUSIZE 72

#define PREFIX NAME"> "

struct __args {
    int split; //has a splitted string been passed
    size_t size;
    char **ptr;
};

typedef struct {
    bool log; //log game output into files
    bool debug; //log debugging info and output of scripts to stdout
    struct __args environment; // global environment variables
    string path;
} config;

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
    struct __args environment;

    struct __game_wine wine;
    struct __game_scripts scripts;
} game_config;

gt_error get_game_paths(string *, string *, string, string);

gt_error get_config(config*, string);

gt_error get_game_config(game_config*, string*, string, string);

#endif
