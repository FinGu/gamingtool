#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "utils.h"
#include "filesys.h"

//to be used somewhere else later
gt_error create_parser(cJSON **out, char *data){
    cJSON *cout = cJSON_Parse(data);

    if(!cout){
        return failed_to_parse;
    }

    *out = cout;

    return ok;
}

gt_error create_config(string* data){
    char *buf;
    cJSON *config = cJSON_CreateObject(), *obj;
    gt_error err = ok;

    if(!config){
        err = cjson_failure;
        goto out;
    }

    obj = cJSON_CreateBool(0);

    cJSON_AddItemToObject(config, "log", obj);

    cJSON_AddItemToObject(config, "debug", obj);

    buf = cJSON_Print(config);

    if(!buf){
        err = cjson_failure;
    }

    *data = (string){strlen(buf), buf};

    out:
    cJSON_Delete(config);

    return err;
}

gt_error parse_config(config *out, char *data){
    cJSON *config, *handle;
    gt_error err = ok;

    config = handle = NULL;
    
    if((err = create_parser(&config, data))){
        goto out;
    }

    handle = cJSON_GetObjectItem(config, "log");

    if(!cJSON_IsBool(handle)){
        err = failed_to_parse;
        goto out;
    }

    out->log = handle->valueint;

    handle = cJSON_GetObjectItem(config, "debug");

    if(!cJSON_IsBool(handle)){
        err = failed_to_parse;
        goto out;
    }

    out->debug = handle->valueint;

    out:
    cJSON_Delete(config);

    return err;
}

//messy
gt_error parse_game_config(game_config *out, char *data){
    gt_error err = ok;
    cJSON *config, *handle, *in;

    config = handle = in = NULL;

    if((err = create_parser(&config, data))){
        goto out;
    }

    //name is handled somewhere else
    
    handle = cJSON_GetObjectItem(config, "path");

    if(!cJSON_IsString(handle)){
        err = failed_to_parse;
        goto out;
    }

    out->path = strdup(handle->valuestring);
    //the path of the game is necessary 

    handle = cJSON_GetObjectItem(config, "arguments");

    if(cJSON_IsString(handle)){
        out->arguments = strdup(handle->valuestring);
    }

    handle = cJSON_GetObjectItem(config, "wine");

    in = cJSON_GetObjectItem(handle, "version");

    if(cJSON_IsString(in)){
        out->wine.version = strdup(in->valuestring);
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

    out:
    cJSON_Delete(config);

    return err;
}

void free_game_config(game_config *to_free){
    free(to_free->name);
    free(to_free->path);
    free(to_free->arguments);
    free(to_free->wine.version);
}
