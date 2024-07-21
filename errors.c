#include "errors.h"
#include <stdio.h>

#include "config.h"

void print_error(gt_error error){
    //if(!error){
        //return;
    //}

    static struct { gt_error err; char* msg; } err_table[] = {
        {ok, NULL},
        {invalid_input, "Invalid input"},
        {failed_to_open, "Failed to open the file/dir"},
        {failed_to_read, "Failed to read the file"},
        {failed_to_write, "Failed to write to the file"},
        {failed_to_delete, "Failed to delete a/the file"},
        {failed_to_create_dir, "Failed to create directories"},
        {failed_to_execute, "Failed to execute ( no execute permissions )"},
        {home_not_found, "The environment variable HOME wasn't found, please create it before proceeding"},
        {failed_to_parse, "Failed to parse the file"},
        {cjson_failure, "cJSON failed, report this"},
        {game_not_found, "The game path couldn't be accessed"}, 
        {couldnt_find_wine, "Couldn't find the wine version"},
        {failed_to_start, "Failed to start process"},
        {game_already_exists, "Game already exists"},
        {wine_not_found, "Wine version not found"},
        {curl_bad_instance, "Could not instantiate libcurl"},
        {target_not_found, "Install target wasn't found, check the available packages at https://github.com/fingu/gamingtool-library"},
        {target_already_exists, "Wine or game already exists"},
    };

    printf(PREFIX"%s\n", err_table[error].msg);
}
