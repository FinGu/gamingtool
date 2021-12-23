#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "alloc.h"

void *scalloc(size_t num, size_t size){ //safe calloc
    void *out = calloc(num, size);

    if(!out){
        puts(PREFIX"Fatal issue, failed to allocate memory");
        exit(-1);
    }

    return out;
}

string salloc(size_t size){ //string alloc
    char *mem = scalloc(size+1, sizeof(char));

    return (string){size, mem};
}

void sfree(string to_free){
    free(to_free.ptr);
}

char *copycatalloc(size_t size, char *copy, char *cat){
    char *out = scalloc(size+1, sizeof(char));

    copycat(out, copy, cat); 

    return out;
}
