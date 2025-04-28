#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "utils.h"

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

    obj = cJSON_CreateArray();

    cJSON_AddItemToObject(config, "environment", obj);

    buf = cJSON_Print(config);

    if(!buf){
        err = cjson_failure;
        goto out;
    }

    outbuf = str_view(strlen(buf), buf);

    *data = outbuf; 

    out:
    cJSON_Delete(config);

    return err;
}

gt_error create_game_config(string* data, game_config in){
    char *buf;
    size_t i;
    string outbuf = str_alloc(0);
    cJSON *config = cJSON_CreateObject(), *obj, *inel;
    gt_error err = ok;

    if(!config){
        err = cjson_failure;
        goto out;
    }

    //name isn't part of the cfg

    if(in.path){
        obj = cJSON_CreateString(in.path);

        cJSON_AddItemToObject(config, "path", obj);
    }

    if(in.arguments.size){
        obj = cJSON_CreateArray();

        for(i = 0; i < in.arguments.size; ++i){
            inel = cJSON_CreateString(in.arguments.ptr[i]);

            cJSON_AddItemToArray(obj, inel) ;
        }

        cJSON_AddItemToObject(config, "arguments", obj);
    }

    if(in.environment.size){
        obj = cJSON_CreateArray();

        for(i = 0; i < in.environment.size; ++i){
            inel = cJSON_CreateString(in.environment.ptr[i]);

            cJSON_AddItemToArray(obj, inel);
        }

        cJSON_AddItemToObject(config, "environment", obj);
    }

    if(in.wine.version){
        obj = cJSON_CreateObject();

        inel = cJSON_CreateString(in.wine.version);

        cJSON_AddItemToObject(obj, "version", inel);
        
        cJSON_AddItemToObject(config, "wine", obj);
    }

    obj = cJSON_CreateObject();

    inel = cJSON_CreateBool(in.scripts.prelaunch);

    cJSON_AddItemToObject(obj, "prelaunch", inel);

    inel = cJSON_CreateBool(in.scripts.postlaunch);

    cJSON_AddItemToObject(obj, "postlaunch", inel);

    cJSON_AddItemToObject(config, "scripts", obj);

    buf = cJSON_Print(config);

    if(!buf){
        err = cjson_failure;
        goto out;
    }

    outbuf = str_view(strlen(buf), buf);

    *data = outbuf;

    out:
    cJSON_Delete(config);

    return err;
}

gt_error parse_config(config *out, string path, char *data){
    int sz, i = 0;
    cJSON *config, *handle, *in;
    gt_error err = ok;

    config = handle = in = NULL;

    out->path = path;
    
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

    handle = cJSON_GetObjectItem(config, "environment");

    sz = cJSON_GetArraySize(handle);

    if(sz){
        out->environment = (struct __args){0, sz, calloc(sz, sizeof(char*))};

        cJSON_ArrayForEach(in, handle)
        {
            if(!cJSON_IsString(in)){
                continue;
            }
            
            out->environment.ptr[i++] = strdup(in->valuestring);
        }
    }

    out:
    cJSON_Delete(config);

    return err;
}

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
        out->arguments = (struct __args){0, sz, calloc(sz, sizeof(char*))};
                
        cJSON_ArrayForEach(in, handle)
        {
            if(!cJSON_IsString(in)){
                continue;
            }
            
            out->arguments.ptr[i++] = strdup(in->valuestring);
        } 
    }

    handle = cJSON_GetObjectItem(config, "environment");

    sz = cJSON_GetArraySize(handle);

    if(sz){
        i = 0;

        out->environment = (struct __args){0, sz, calloc(sz, sizeof(char*))};

        cJSON_ArrayForEach(in, handle)
        {
            if(!cJSON_IsString(in)){
                continue;
            }
            
            out->environment.ptr[i++] = strdup(in->valuestring);
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

    free(to_free->wine.version);

    if(to_free->arguments.split){
        free_split((__split_out){.size = to_free->arguments.size, .ptr = to_free->arguments.ptr});
    } else {
        for(; i < to_free->arguments.size; ++i){
            free(to_free->arguments.ptr[i]);
        }
        free(to_free->arguments.ptr); 
    }

    if(to_free->environment.split){
        free_split((__split_out){.size = to_free->environment.size, .ptr = to_free->environment.ptr});
        return;
    } 

    for(i = 0; i < to_free->environment.size; ++i){
        free(to_free->environment.ptr[i]);
    }

    if(to_free->environment.size){
        free(to_free->environment.ptr);
    }
}

void free_config(config *to_free){
    int i;

    for(i = 0; i < to_free->environment.size; ++i){
        free(to_free->environment.ptr[i]);
    }

    if(to_free->environment.size){
        free(to_free->environment.ptr);
    }

    str_free(&to_free->path);
}
