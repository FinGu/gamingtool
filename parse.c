#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "config.h"

//to be used somewhere else later
gt_error create_parser(cJSON **out, char *data){
    cJSON *cout = cJSON_Parse(data);
    //char *errptr = NULL;

    if(!cout){
        /* errptr = (char*)cJSON_GetErrorPtr();

        if(errptr){
            printf(PREFIX"%s", errptr);
        } */
        return failed_to_parse;
    }

    *out = cout;

    return ok;
}

//messy
gt_error parse_game(game_config *out, char *data){
    gt_error err = ok;

    cJSON *config, *handle, *in;

    config = handle = in = NULL;

    if((err = create_parser(&config, data))){
        return err;
    }

    //name is handled somewhere else
    
    handle = cJSON_GetObjectItem(config, "executable");

    if(cJSON_IsString(handle)){
        out->executable = strdup(handle->valuestring);
    }

    handle = cJSON_GetObjectItem(config, "arguments");

    if(cJSON_IsString(handle)){
        out->arguments = strdup(handle->valuestring);
    }

    handle = cJSON_GetObjectItem(config, "wine");

    in = cJSON_GetObjectItem(handle, "enabled");

    out->wine.enabled = ((cJSON_IsBool(in)) ? in->valueint : 0);

    if(out->wine.enabled){
        in = cJSON_GetObjectItem(handle, "version");

        if(cJSON_IsString(in)){
            out->wine.version = strdup(in->valuestring);
        }    
    } 

    handle = cJSON_GetObjectItem(config, "scripts");

    in = cJSON_GetObjectItem(handle, "prelaunch");

    if(cJSON_IsBool(in)){
        out->scripts.prelaunch = in->valueint;
    } 

    in = cJSON_GetObjectItem(handle, "postlaunch");

    if(cJSON_IsBool(in)){
        out->scripts.postlaunch = in->valueint;
    }

    handle = cJSON_GetObjectItem(config, "log");

    if(cJSON_IsBool(handle)){
        out->log = handle->valueint;
    }
    
    cJSON_Delete(config);

    return err;
}

void free_game(game_config *to_free){
    free(to_free->name);
    free(to_free->executable);
    free(to_free->arguments);
    free(to_free->wine.version);
}
