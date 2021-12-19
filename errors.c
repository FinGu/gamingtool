#include <stdio.h>
#include "config.h"

void print_error(gt_error error){
    //if(!error){
        //return;
    //}

    struct { gt_error err; char* msg; } err_table[] = {
        {ok, NULL},
        {invalid_input, "Invalid input"},
        {failed_to_open, "Failed to open the file"},
        {failed_to_read, "Failed to read the file"},
        {failed_to_write, "Failed to write to the file"},
        {failed_to_create_dir, "Failed to create directories"},
        {failed_to_execute, "Failed to execute ( no execute permissions )"},
        {home_not_found, "The environment variable HOME wasn't found, please create it before proceeding"},
        {failed_to_parse, "Failed to parse the file"},
        {cjson_failure, "cJSON failed, report this"},
        {game_not_found, "The game path couldn't be accessed"}, 
        {couldnt_find_wine, "Couldn't find the wine version"},
        {failed_to_start, "Failed to start process"},
        {process_returned_error, "Process returned an error"}
    };

    printf(PREFIX"%s\n", err_table[error].msg);
}
