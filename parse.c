#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"

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
    string outbuf = str_alloc(0);
    cJSON *config = cJSON_CreateObject(), *obj;
    gt_error err = ok;

    if(!config){
        err = cjson_failure;
        goto out;
    }

    obj = cJSON_CreateBool(false);

    cJSON_AddItemToObject(config, "log", obj);

    obj = cJSON_CreateBool(false);

    cJSON_AddItemToObject(config, "debug", obj);

    buf = cJSON_Print(config);

    if(!buf){
        err = cjson_failure;
        goto out;
    }

    outbuf = (string){.len = strlen(buf), .ptr = buf};

    *data = outbuf;

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
    int i = 0, sz;

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

    handle = cJSON_GetObjectItem(config, "wine");

    in = cJSON_GetObjectItem(handle, "version");

    if(cJSON_IsString(in)){
        out->wine.version = strdup(in->valuestring);
    }

    handle = cJSON_GetObjectItem(config, "arguments");

    sz = cJSON_GetArraySize(handle);

    if(out->wine.version){
        ++sz;
        i = 1;
    }

    if(sz){
        out->arguments = (struct __args){sz, calloc(sz, sizeof(char*))};
                
        cJSON_ArrayForEach(in, handle)
        {
            if(!cJSON_IsString(in)){
                continue;
            }

            out->arguments.ptr[i++] = strdup(in->valuestring);
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

    out:
    cJSON_Delete(config);

    return err;
}

void free_game_config(game_config *to_free){
    int i = to_free->wine.version != NULL; // avoid freeing the first arg if a wine version is set 

    free(to_free->name);
    free(to_free->path);

    for(; i < to_free->arguments.size; ++i){
        free(to_free->arguments.ptr[i]);
    }

    free(to_free->arguments.ptr);

    free(to_free->wine.version);
}
