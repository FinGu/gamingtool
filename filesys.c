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

    bool first_entry = true;
    
    while((dd = readdir(d))){
        //we want to see hidden files too
        if(strcmp(dd->d_name, ".") == 0 || strcmp(dd->d_name, "..") == 0){ 
            continue; 
        } 

        if(!first_entry){
            printf(",");
        }

        printf("%s", dd->d_name);

        first_entry = false;
    }

    putchar('\n');

    closedir(d);

    return ok;  
}

gt_error delete_files_in_folder(char *folder){
    int st;
    gt_error err = ok;
    DIR *d = opendir(folder);
    struct dirent *dd;
    size_t tmplen = strlen(folder);
    char *tmpp = NULL;

    if(!d){
        err = failed_to_open;
        goto out;
    }
    
    while((dd = readdir(d))){
        if(strcmp(dd->d_name, ".") == 0 || strcmp(dd->d_name, "..") == 0){ 
            continue; 
        }

        tmplen += strlen(dd->d_name) + (dd->d_type == DT_DIR) + 2; //1 for /, 1 for term

        tmpp = calloc(tmplen, sizeof(char));

        OUTMEM(tmpp);

        strcpy(tmpp, folder);

        strcat(tmpp, "/");

        strcat(tmpp, dd->d_name);

        if(dd->d_type == DT_DIR){
            if((err = delete_files_in_folder(tmpp))){
                goto out;
            }

            if((st = rmdir(tmpp)) == -1){
                err = failed_to_delete;
                goto out;
            }
        } else {
            if((st = remove(tmpp)) == -1){
                err = failed_to_delete;
                goto out;
            }
        }

        free(tmpp);
        
        tmpp = NULL;
    }

out:
    if(tmpp){
        free(tmpp);
    }

    closedir(d);


    return err;
}

gt_error read_config(size_t bufsize, char *buf, string folder){ 
    int fd;
    gt_error err = ok;
    size_t nlen = str_len(&folder) + 6; //6 for config

    string location = str_alloc(nlen);

    str_append_multiple(&location, 2, folder, str_view(6, "config"));

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
    size_t nlen = str_len(&folder) + 6 + str_len(&game); // 6 for game//
    
    cout = str_alloc(nlen);

    str_append_multiple(&cout, 4, folder, str_view(5, "game/"), game, str_view(1, "/"));

    if(!can_access(str_raw_p(&cout), S_IFDIR)){ 
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

    slen = hlen + dlen + 1;  // for /

    folder = str_alloc(slen);

    str_append_multiple(&folder, 3, str_view(hlen, home), str_view(dlen, DIRNAME), str_view(1, "/"));

    tmpp = str_raw_p(&folder);

    if(!can_access(tmpp, S_IFDIR)){
        if(!__mkdir(tmpp)){
            err = failed_to_create_dir;
            goto out;
        }

        nfolder = str_alloc(slen + 4); //+4 for the subfolder name
                                    
        str_append_multiple(&nfolder, 2, folder, str_view(4, "game"));

        tmpp = str_raw_p(&nfolder); //this pointer shouldnt change as in this context realloc isnt called

        __mkdir(tmpp); //shouldn't fail

        str_clear(&nfolder, 4); //clears game

        str_append(&nfolder, str_view(4, "wine"));

        __mkdir(tmpp);
        
        str_clear(&nfolder, 4); //clears wine
        
        str_append(&nfolder, str_view(3, "log"));

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

