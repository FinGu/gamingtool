#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <dirent.h>

#include "filesys.h"
#include "config.h"
#include "utils.h"

int can_access(char *file, int folder){
    struct stat s;

    int exists = lstat(file, &s) == 0;

    if((!exists) || (folder && !S_ISDIR(s.st_mode))){
        return 0;
    }

    return 1;
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

gt_error read_write_config(int mode, size_t size, char *buf, char *folder){
    gt_error err = ok;
    int nlen = strlen(folder) + 6; //6 for config
    char *location = copycatalloc(nlen, folder, "config");

    err = read_write_file(mode, size, buf, location);

    free(location);

    return err; 
}

gt_error get_game_folder(char **out, char *folder, char *game){
    char *cout;
    int nlen = strlen(folder) + 6 + strlen(game); // 6 for game//
    
    cout = smalloc(nlen);

    sprintf(cout, "%sgame/%s/", folder, game);

    if(!can_access(cout, 1)){
        free(cout);
        return failed_to_open;
    }

    *out = cout;

    return ok;
}

gt_error get_create_folder(char **out){
    char *home = getenv("HOME"), *folder, *nfolder;
    int slen;

    if(!home){
        return home_not_found;
    }

    slen = strlen(home) + NSIZE + 3; //+1 for . and +2 for /

    folder = copycatalloc(slen, home, DIRNAME);
    
    pstrcat(folder, "/");

    *out = folder; 

    if(can_access(folder, 1)){
        return ok;
    }

    if(mkdir(folder, S_IRWXU | S_IRWXG | S_IRWXO) == -1){
        free(folder);
        return failed_to_create_dir;
    }

    nfolder = copycatalloc(slen + 4, folder, "game"); // + 4 for the subfolder name

    mkdir(nfolder, S_IRWXU | S_IRWXG | S_IRWXO); //shouldn't fail

    memset(&nfolder[slen], 0, 4); //clears game

    pstrcat(nfolder, "wine");

    mkdir(nfolder, S_IRWXU | S_IRWXG | S_IRWXO);

    free(nfolder);

    return ok;
}

