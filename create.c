#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "create.h"
#include "errors.h"
#include "filesys.h"
#include "parse.h"
#include "utils.h"
#include "list.h"

gt_error reask_for_wine(game_config *, string, char *, size_t);

char *__strdup(size_t*, char*);

gt_error create(config *cfg, string folder, string game){
    int fd = -1;
    size_t fpsz, len = 0;
    string filepath, tmp = {};
    gt_error err = ok;
    char *tmpp, buf[BUFSIZE] = {0};
    string pgamecfg;
    game_config gamecfg = {0}; 
    __split_out spl = {0};
    
    printf(PREFIX"Game's path: ");

    (void)fgets(buf, BUFSIZE, stdin);

    gamecfg.path = __strdup(&len, buf);

    gamecfg.path[len-1] = '\0'; //ignores the last character: '\n'

    printf("Launch arguments ( separated by commas ) ( leave empty for none ): ");
    
    (void)fgets(buf, BUFSIZE, stdin);

    if(*buf != '\n'){
        len = strlen(buf);

        buf[len-1] = '\0';

        spl = split(',', str_view(len, buf));

        gamecfg.arguments = (struct __args){.split = 1, .size = spl.size, .ptr = spl.ptr};
    } 

    printf("Environment variables ( A=b, ...) ( leave empty for none ): ");
    
    (void)fgets(buf, BUFSIZE, stdin);

    if(*buf != '\n'){
        len = strlen(buf);

        buf[len-1] = '\0';

        spl = split(',', str_view(len, buf));

        gamecfg.environment = (struct __args){.split = 1, .size = spl.size, .ptr = spl.ptr};
    }

    printf("Wine version ( leave empty to not use wine ): ");

    (void)fgets(buf, BUFSIZE, stdin);

    if(*buf != '\n'){
        gamecfg.wine.version = __strdup(&len, buf);

        gamecfg.wine.version[len-1] = '\0';

        if(find_wine(&tmp, folder, gamecfg.wine.version) != ok){
            free(gamecfg.wine.version);
            gamecfg.wine.version = NULL;

            if((err = reask_for_wine(&gamecfg, folder, buf, BUFSIZE))){
                goto out2;
            }
        }
    }

    str_free(&tmp);

    printf("Prelaunch script ( Y/n ): ");

    gamecfg.scripts.prelaunch = (toupper(getchar()) == 'Y');

    stdin->_IO_read_ptr = stdin->_IO_read_end; // works better for my needs ( aka using the enter key to skip the rest of the form )
    
    printf("Postlaunch script ( Y/n ): ");

    gamecfg.scripts.postlaunch = (toupper(getchar()) == 'Y');

    if((err = create_game_config(&pgamecfg, gamecfg))){
        goto out;
    }

    fpsz = str_len(&folder) + str_len(&game)+ 12; //5 for game/, 7 for /config

    filepath = str_alloc(fpsz);

    str_append_multiple(&filepath, 3, folder, str_view(5, "game/"), game);

    tmpp = str_raw_p(&filepath); 

    if(can_access(tmpp, S_IFDIR)){
        err = game_already_exists;
        goto out;
    }

    if(!__mkdir(tmpp)){
        err = failed_to_create_dir;
        goto out;
    }

    str_append(&filepath, str_view(7, "/config")); 

    if((fd = open(str_raw_p(&filepath), O_CREAT | O_RDWR, FILE_PERM)) == -1){
        err = failed_to_open;
        goto out;
    }

    if(write(fd, str_raw_p(&pgamecfg), str_len(&pgamecfg)) <= 0){
        err = failed_to_write;
        goto out;
    }

    out:
    str_free(&pgamecfg);
    str_free(&filepath); 

    out2:
    free_game_config(&gamecfg);
    close(fd);

    return err;
}

char *__strdup(size_t *ln, char *in){
    size_t len = strlen(in);
    char* ptr = malloc(len+1);

    if(!ptr) {
        return NULL;
    }

    strcpy(ptr, in);

    *ln = len; 

    return ptr;
}

gt_error reask_for_wine(game_config *gamecfg, string folder, char *buf, size_t buf_size){
    string tmp;

    size_t len = 0;
    gt_error err = ok; 

    printf("Wine version not found\nAvailable versions are: ");

    list(folder, "wine", false); 

    printf("Try again: ");

    (void)fgets(buf, buf_size, stdin);

    if(*buf == '\n'){
        return err;
    }

    gamecfg->wine.version = __strdup(&len, buf);

    gamecfg->wine.version[len-1] = '\0';

    if(find_wine(&tmp, folder, gamecfg->wine.version) != ok){
        err = wine_not_found;
    } else{
        str_free(&tmp);
    }
    
    return err;
}
