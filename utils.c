#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <mimalloc.h>

#include "utils.h"

void escapeshellargs(string* out, string in){
    size_t len, i, n, j;
    char *buf;

    i = n = 0;

    for(; i < in.len; ++i){
        if(in.ptr[i] == '\''){
            ++n; 
        }
    }

    len = (in.len-n) + n * 4;

    buf = smalloc(len);
    
    //buf[0] = '\'';
    
    for(i = j = 0; i < in.len; i++){
        /* if(in.len == i){
            break;
        } */

        if(in.ptr[i] == '\''){
            buf[j] = '\'';
            buf[j+1] = '\\';
            buf[j+2] = '\'';
            buf[j+3] = '\'';
            j += 4;
        } else{
            buf[j] = in.ptr[i];
            ++j;
        }
    }

    *out = (string){len, buf};
}

gt_error prun(char* process, int log){
    char buf[BUFSIZE];
    FILE* file = popen(process, "r");
    
    if(!file){
        return failed_to_start;
    }

    while(fgets(buf, BUFSIZE, file)){
        if(log){
            puts(buf);
        }
    }

    pclose(file);

    return ok;
} 

void *cmalloc(size_t size){ //custom malloc
    return mi_malloc(size);
}
void *smalloc(size_t size){ //string malloc
    char *out = cmalloc(sizeof(char)*(size+1));

    if(!out){
        puts(PREFIX"Fatal issue, failed to allocate memory");
        exit(-1);
    }

    out[size] = '\0';

    return out;
}

void cfree(void *p){ //custom free
    mi_free(p);
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
