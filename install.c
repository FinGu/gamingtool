#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "delete.h"
#include "errors.h"
#include "filesys.h"
#include "parse.h"

#include "utils.h"

gt_error unpack(string);

gt_error download_to_file(char *, char*);

gt_error install(config *cfg, string folder, string type, string name){
    size_t targetsz, targeturlsz, name_len;
    string target, targeturl;

    gt_error err = ok;
    char *tmpp, *tmpp2;

    name_len = str_len(&name);

    targetsz = str_len(&folder) + 5 + name_len + 4; //5 for game/ or wine/, 4 for zip 
                                                    
    target = str_alloc(targetsz);

    str_append_multiple(&target, 4, folder, type, str_view(1, "/"), name);

    tmpp = str_raw_p(&target);

    if(can_access(tmpp, S_IFDIR)){
        err = target_already_exists;
        goto out;
    }

    str_append(&target, str_view(4, ".zip"));

    targeturlsz = LUSIZE + 5 + name_len + 4; //5 for game/ or wine/ and 4 for .zip

    targeturl = str_alloc(targeturlsz);

    str_append_multiple(&targeturl, 5, str_view(LUSIZE, LIBURL), type, str_view(1, "/"), name, str_view(4, ".zip"));

    tmpp2 = str_raw_p(&targeturl);

    if((err = download_to_file(tmpp2, tmpp)) != ok){
        unlink(tmpp);
        goto out2;
    }

    puts("\n"PREFIX"Unpacking..");

    err = unpack(target);

out2:
    str_free(&targeturl);
out:
    str_free(&target);

    return err;
}

gt_error unpack(string target){
    gt_error err = ok;

    int code;
    size_t cmdsz;
    string cmd;

    FILE *fp;

    char bufi[1024];

    cmdsz = 6 + str_len(&target)*2 + 4; //6 for unzip, 4 for -d

    cmd = str_alloc(cmdsz);

    str_clear(&target, 4);

    str_append_multiple(&cmd, 4, str_view(6, "unzip "), target, str_view(4, " -d "), target);

    if((fp = popen(str_raw_p(&cmd), "r")) == NULL){
        err = failed_to_execute;
        goto out;
    }
    
    while(fgets(bufi, sizeof(bufi), fp) != NULL);

    code = pclose(fp);

    if(code == -1 || !WIFEXITED(code)){
        err = failed_to_execute;
        goto out;
    }

    str_append(&target, str_view(4, ".zip"));

    unlink(str_raw_p(&target));
out:
    str_free(&cmd);

    return err;
}

int progress_bar(void *bar, curl_off_t t, curl_off_t d, curl_off_t ultotal, curl_off_t ulnow) {
    curl_off_t *ldptr = bar;

    if(*ldptr == d || d == 0){
        return 0;
    }

    printf("\r"PREFIX"%ld / %ld (%.2f %%)", d, t, d*100.0/t);

    fflush(stdout);

    *ldptr = d;

    return 0;
}

gt_error download_to_file(char *url, char *path){
    CURL *curl;
    CURLcode res;
    FILE *fp;
    long code = 0;
    curl_off_t lastd;

    if(!(curl = curl_easy_init())){
        return curl_bad_instance;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_bar);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &lastd);

    fp = fopen(path, "wb");

    if(!fp){
        curl_easy_cleanup(curl);
        return failed_to_open;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    
    res = curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    curl_easy_cleanup(curl);

    fclose(fp);

    return ((res == CURLE_OK && code == 200) ? ok : target_not_found);
}
