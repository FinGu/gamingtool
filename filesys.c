#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <dirent.h>

#include "filesys.h"
#include "utils.h"

int can_access(char *file, int perm){
    struct stat s;

    int exists = lstat(file, &s) == 0;
    
    if(exists && (perm == 0 || (s.st_mode & perm))) {
        return 1;
    }

    return 0;
}

gt_error read_write_file(int mode, size_t size, char *buf, char *file_location){
    gt_error err = ok;
    int i, file;

    //O_WRONLY = 1
    //O_RDONLY = 0 

    file = open(file_location, mode);

    if(file == -1) {
        err = failed_to_open;
        goto out;
    }

    i = ((mode) ? write(file, buf, size) : read(file, buf, size));

    if(i <= 0){
        err = failed_to_read;
        goto out;
    }

    buf[size] = '\0';

    out:
    close(file);

    return err;
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

gt_error read_write_config(int mode, size_t bufsize, char *buf, string folder){ 
    gt_error err = ok;
    size_t nlen = folder.len + 6; //6 for config

    char *location = copycatalloc(nlen, folder.ptr, "config");

    err = read_write_file(mode, bufsize, buf, location);

    free(location);

    return err; 
}

gt_error get_game_folder(string *out, string folder, string game){
    gt_error err = ok;
    string cout;
    size_t nlen = folder.len + 6 + game.len; // 6 for game//
    
    cout = salloc(nlen);

    sprintf(cout.ptr, "%sgame/%s/", folder.ptr, game.ptr);

    if(!can_access(cout.ptr, S_IFDIR)){ //probably should remove this as read_write_file does this job
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
    
    pstrcat(folder.ptr, "/");

    if(!can_access(folder.ptr, S_IFDIR)){
        if(mkdir(folder.ptr, S_IRWXU | S_IRWXG | S_IRWXO) == -1){
            err = failed_to_create_dir;
            goto out;
        }

        nfolder = copycatalloc(slen + 4, folder.ptr, "game"); // + 4 for the subfolder name

        mkdir(nfolder, S_IRWXU | S_IRWXG | S_IRWXO); //shouldn't fail

        memset(&nfolder[slen], 0, 4); //clears game

        pstrcat(nfolder, "wine");

        mkdir(nfolder, S_IRWXU | S_IRWXG | S_IRWXO);

        free(nfolder);
    }

    *out = folder;

    out:
    if(err){
        sfree(folder);
    }

    return err;
}

