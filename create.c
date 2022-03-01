#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "create.h"
#include "filesys.h"
#include "parse.h"
#include "utils.h"

char *__strdup(size_t*, char*);

gt_error create(config *cfg, string folder, string game){
    int fd = -1;
    size_t fpsz, len = 0;
    string filepath;
    gt_error err = ok;
    char c, *tmpp, buf[BUFSIZE] = {0};
    string pgamecfg;
    game_config gamecfg = {0};
    __split_out spl = {0};
    
    fpsz = str_len(&folder) + str_len(&game)+ 12; //5 for game/, 7 for /config

    filepath = str_alloc(fpsz);

    str_append_s(&filepath, folder);

    str_append_p(&filepath, 5, "game/");

    str_append_s(&filepath, game); 

    //assert(game[game.len-1] != '/');

    tmpp = str_raw_p(&filepath); 

    if(can_access(tmpp, S_IFDIR)){
        err = game_already_exists;
        goto out;
    }

    if(!__mkdir(tmpp)){
        err = failed_to_create_dir;
        goto out;
    }

    str_append_p(&filepath, 7, "/config");

    fputs(PREFIX"Game's path: ", stdout);

    fgets(buf, BUFSIZE, stdin);

    gamecfg.path = __strdup(&len, buf);

    gamecfg.path[len-1] = '\0'; //ignores the last character: '\n'

    /* if((bsz = read(STDIN_FILENO, buf, BUFSIZE))){
        memcpy(gamecfg.path, buf, bsz);

        buf[bsz-1] = '\0';
    } */
    
    fputs("Launch arguments ( separated by commas ) ( leave empty for none ): ", stdout);
    
    fgets(buf, BUFSIZE, stdin);

    if(*buf != '\n'){
        len = strlen(buf);

        buf[len-1] = '\0';

        spl = split(',', str_view(len, buf));

        gamecfg.arguments = (struct __args){.split = 1, .size = spl.size, .ptr = spl.ptr};
    }

    fputs("Wine version ( leave empty to not use wine ): ", stdout);

    fgets(buf, BUFSIZE, stdin);

    if(*buf != '\n'){
        gamecfg.wine.version = __strdup(&len, buf);

        gamecfg.wine.version[len-1] = '\0';
    }

    fputs("Prelaunch script ( Y/n ): ", stdout);

    gamecfg.scripts.prelaunch = (toupper(getchar()) == 'Y');

    while((c = getchar()) != '\n' && c != EOF); //flushes stdin
    
    fputs("Postlaunch script ( Y/n ): ", stdout);

    gamecfg.scripts.postlaunch = (toupper(getchar()) == 'Y');

    if((err = create_game_config(&pgamecfg, gamecfg))){
        goto out2;
    }

    if((fd = open(str_raw_p(&filepath), O_CREAT | O_RDWR, FILE_PERM)) == -1){
        err = failed_to_open;
        goto out2;
    }

    if(write(fd, str_raw_p(&pgamecfg), str_len(&pgamecfg)) <= 0){
        err = failed_to_write;
        goto out2;
    }

    out2:
    str_free(&pgamecfg);
    free_game_config(&gamecfg);
    close(fd);

    out:
    str_free(&filepath);

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
