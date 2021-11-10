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
    cJSON *config = cJSON_CreateObject(), *log;
    gt_error err = ok;

    if(!config){
        err = cjson_failure;
        goto out;
    }

    log = cJSON_CreateBool(0);

    cJSON_AddItemToObject(config, "log", log);

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

    out:
    cJSON_Delete(config);

    return err;
}

//messy
gt_error parse_game_config(game_config *out, char *data){
    gt_error err = ok;
    cJSON *config, *handle, *in;
    int len;

    config = handle = in = NULL;

    if((err = create_parser(&config, data))){
        goto out;
    }

    //name is handled somewhere else
    
    handle = cJSON_GetObjectItem(config, "folder");

    if(!cJSON_IsString(handle)){
        err = failed_to_parse;
        goto out;
    }
    
    len = strlen(handle->valuestring);
    
    if(handle->valuestring[len] != '/'){
        out->folder = copycatalloc(len+1, handle->valuestring, "/"); //1 for /
    } else {
        out->folder = strdup(handle->valuestring);
    }

    handle = cJSON_GetObjectItem(config, "executable");

    if(!cJSON_IsString(handle)){
        err = failed_to_parse;
        goto out;
    }

    out->executable = strdup(handle->valuestring);

    //both folder and executable are necessary for a game to run xd

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

    out:
    cJSON_Delete(config);

    return err;
}

void free_game_config(game_config *to_free){
    free(to_free->name);
    free(to_free->folder);
    free(to_free->executable);
    free(to_free->arguments);
    free(to_free->wine.version);
}
