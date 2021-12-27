#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

string str_alloc(size_t size){
    if(size == 0){
        return (string){0, 0, 0, NULL};
    }

    size_t sz = size + __TERM;

    char *ptr = malloc(sizeof(char) * sz);

    OUTMEM(ptr);

    return (string){0, sz, 0, ptr};
}

void str_realloc(string *in, size_t nsize){
    int ns = nsize + __TERM;

    char *reptr = realloc(in->ptr, sizeof(char) * ns);
    
    OUTMEM(reptr); //free(in->ptr);

    in->size = ns;
    in->ptr = reptr;
}

void str_append_s(string *in, string to_append){
    int appendlen_asint = to_append.len,
        insize_noterm = in->size - __TERM,
        chk = insize_noterm - appendlen_asint;

    if(chk <= appendlen_asint){
        if(in->flag){ // one use only flag
            str_realloc(in, insize_noterm + appendlen_asint);
        } else {
            str_realloc(in, ((chk < 0) ? -chk : chk) + insize_noterm);

            in->flag = 1;
        }
    }

    memcpy(in->ptr + in->len, to_append.ptr, to_append.len + __TERM);
    
    in->len += to_append.len;
}

void str_append_p(string *in, size_t len, char *to_append){
    string _tmpb = {len, 0, 0, to_append}; // size=0
    
    str_append_s(in, _tmpb);
}

void str_clear(string *in, size_t num){
    char *ptr = &str_raw_p(in)[in->len - num];

    memset(ptr, 0, num);

    in->len -= num;
}

size_t str_len(string *in){
    return in->len;
}

char *str_raw_p(string *in){
    return in->ptr; 
}

void str_free(string *in){
    free(in->ptr);

    *in = str_alloc(0);
}

