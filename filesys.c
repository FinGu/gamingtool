#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <dirent.h>

#include "filesys.h"
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

    string location = str_alloc(nlen);

    str_append_s(&location, folder);

    str_append_p(&location, 6, "config");

    fd = open(str_raw_p(&location), O_RDONLY, FILE_PERM);
    
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
    str_free(&location);
    close(fd);

    return err; 
}

gt_error get_game_folder(string *out, string folder, string game){
    gt_error err = ok;
    string cout;
    size_t nlen = folder.len + 6 + game.len; // 6 for game//
    
    cout = str_alloc(nlen);

    str_append_s(&cout, folder);

    str_append_p(&cout, 5, "game/");

    str_append_s(&cout, game);

    str_append_p(&cout, 1, "/");

    if(!can_access(cout.ptr, S_IFDIR)){ 
        err = failed_to_open;
        goto out;
    }

    *out = cout;

    out:
    if(err){
        str_free(&cout);
    }

    return err;
}

gt_error get_create_folder(string *out){
    gt_error err = ok;
    char *home = getenv("HOME"), *tmpp;
    string folder, nfolder;
    size_t hlen, dlen, slen;

    if(!home){
        err = home_not_found;
        goto out;
    }

    hlen = strlen(home);

    dlen = NSIZE + 2; //+1 for / and +1 for .

    slen = hlen + dlen; 

    folder = str_alloc(slen);

    str_append_p(&folder, hlen, home);

    str_append_p(&folder, dlen, DIRNAME);

    str_append_p(&folder, 1, "/");

    if(!can_access(folder.ptr, S_IFDIR)){
        if(!__mkdir(folder.ptr)){
            err = failed_to_create_dir;
            goto out;
        }

        nfolder = str_alloc(slen + 4); //+4 for the subfolder name

        str_append_s(&nfolder, folder);

        str_append_p(&nfolder, 4, "game"); 

        tmpp = str_raw_p(&nfolder); //this pointer shouldnt change as in this context realloc isnt called

        __mkdir(tmpp); //shouldn't fail

        str_clear(&nfolder, 4); //clears game

        str_append_p(&nfolder, 4, "wine");

        __mkdir(tmpp);
        
        str_clear(&nfolder, 4); //clears wine

        str_append_p(&nfolder, 3, "log");

        __mkdir(tmpp);

        str_free(&nfolder);
    }

    *out = folder;

    out:
    if(err){
        str_free(&folder);
    }

    return err;
}

