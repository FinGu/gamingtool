#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "config.h"

gt_error prun(char *process, int log){
    FILE *file = popen(process, "r");
    
    if(!file){
        return failed_to_start;
    }

    if(log){
        char buf[BUFSIZE];

        while(fgets(buf, BUFSIZE, file)){
            puts(buf);
        }
    }

    pclose(file);

    return ok;
} 

void *smalloc(size_t size){ //string malloc
    char *out = malloc(sizeof(char)*(size + 1));

    if(!out){
        puts(PREFIX"Fatal issue, failed to allocate memory");
        exit(-1);
    }

    out[size] = '\0';

    return out;
}

void pstrcat(char *dest, char *source){
    int len1 = strlen(dest), len2 = strlen(source);

    memcpy(dest + len1, source, len2 + 1);
}

void pstrcpy(char *dest, char *source){
    int len = strlen(source);

    memcpy(dest, source, len + 1);
}

void copycat(char *buf, char *copy, char *cat){
    pstrcpy(buf, copy);

    pstrcat(buf, cat);
}

char *copycatalloc(size_t size, char *copy, char *cat){
    char *out = smalloc(size);
    
    copycat(out, copy, cat); 

    return out;
}
