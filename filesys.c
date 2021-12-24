#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <dirent.h>

#include "filesys.h"
#include "alloc.h"
#include "utils.h"

bool __mkdir(char *path){
    return mkdir(path, S_IRWXU | S_IRGRP) == 0;
}

bool can_access(char *file, int perm){
    struct stat s;

    int exists = lstat(file, &s) == 0;
    
    if(exists && (perm == 0 || (s.st_mode & perm))) {
        return true;
    }

    return false;
}

gt_error print_files_in_folder(char *folder){
    DIR *d = opendir(folder);
    struct dirent *dd;

    if(!d){
        return failed_to_open;
    }
     
    while((dd = readdir(d))){
        //we want to see hidden files too
        if(strcmp(dd->d_name, ".") == 0 || strcmp(dd->d_name, "..") == 0){ 
            continue; 
        }

         printf("%s,", dd->d_name);
    }

    putchar('\n');

    closedir(d);

    return ok;
}

gt_error read_config(size_t bufsize, char *buf, string folder){ 
    int fd;
    gt_error err = ok;
    size_t nlen = folder.len + 6; //6 for config

    char *location = copycatalloc(nlen, folder.ptr, "config");

    fd = open(location, O_RDONLY, FILE_PERM);
    
    if(fd == -1){
        err = failed_to_open;
        goto out;
    }

    if(read(fd, buf, bufsize) <= 0){
        err = failed_to_read;
        goto out;
    }

    buf[bufsize-1] = '\0';

    out:
    free(location);

    return err; 
}

gt_error get_game_folder(string *out, string folder, string game){
    gt_error err = ok;
    string cout;
    size_t nlen = folder.len + 6 + game.len; // 6 for game//
    
    cout = salloc(nlen);

    sprintf(cout.ptr, "%sgame/%s/", folder.ptr, game.ptr);

    if(!can_access(cout.ptr, S_IFDIR)){ 
        err = failed_to_open;
        goto out;
    }

    *out = cout;

    out:
    if(err){
        sfree(cout);
    }

    return err;
}

gt_error get_create_folder(string *out){
    gt_error err = ok;
    char *home = getenv("HOME"), *nfolder;
    string folder;
    size_t slen;

    if(!home){
        err = home_not_found;
        goto out;
    }

    slen = strlen(home) + NSIZE + 3; //+1 for . and +2 for /

    folder = salloc(slen);

    copycat(folder.ptr, home, DIRNAME);
    
    strcat(folder.ptr, "/");

    if(!can_access(folder.ptr, S_IFDIR)){
        if(!__mkdir(folder.ptr)){
            err = failed_to_create_dir;
            goto out;
        }

        nfolder = copycatalloc(slen + 4, folder.ptr, "game"); // + 4 for the subfolder name

        __mkdir(nfolder); //shouldn't fail

        memset(&nfolder[slen], 0, 4); //clears game

        strcat(nfolder, "wine");

        __mkdir(nfolder);

        memset(&nfolder[slen], 0, 4); //clears wine

        strcat(nfolder, "log");

        __mkdir(nfolder);

        free(nfolder);
    }

    *out = folder;

    out:
    if(err){
        sfree(folder);
    }

    return err;
}

