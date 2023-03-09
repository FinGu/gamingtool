#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

string str_view(size_t len, char* ptr) { //really abused function
    string out;

    out.len = len;
    out.ptr = ptr;

    return out;
}

string str_alloc(size_t size) {
    string out = (string){ 0,0,NULL };

    if (size == 0) {
        return out;
    }

    size_t sz = size + __TERM;

    char* ptr = (char*)malloc(sizeof(char) * sz);

    OUTMEM(ptr);

    memset(ptr, 0, sz);

    out = (string){ 0, sz, ptr };

    return out;
}

void __str_realloc(string* in, size_t nsize) {
    size_t ns = nsize*2; //allocate in blocks

    char* reptr = (char*)realloc(str_raw_p(in), sizeof(char) * ns );  

    OUTMEM(reptr); //free(in->ptr);

    in->size = ns;
    in->ptr = reptr;
}

void str_append(string* in, string to_append) {
    long long length_of_the_original_str = in->len,
        length_to_be_appended = to_append.len,
        size_of_the_original_str = in->size;

    long long available_space = size_of_the_original_str - length_of_the_original_str - __TERM, diff;

    if (length_to_be_appended > available_space) {
        diff = length_to_be_appended - available_space;

        __str_realloc(in, size_of_the_original_str + diff); 
    }

    memcpy(str_raw_p(in) + length_of_the_original_str, str_raw_p(&to_append), length_to_be_appended + __TERM);

    in->len += length_to_be_appended;
}

void str_append_multiple(string *in, size_t num, ...){
    size_t i;
    va_list ptr;

    va_start(ptr, num);

    for(i = 0; i < num; ++i){
        string arg = va_arg(ptr, string);

        str_append(in, arg);
    }

    va_end(ptr);
} 

void str_clear(string* in, size_t num) {
    char* ptr = &str_raw_p(in)[in->len - num];

    memset(ptr, 0, num);

    in->len -= num;
}

string str_sub(string* in, int offset, int length) {
    int start = 0;

    string out = (string){.len = length};

    if(offset >= 0){
        start = offset;
    } else{
        start = in->len + offset;
    }

    if(length <= 0){
        out.len += in->len;
    } 
 
    out.ptr = &str_raw_p(in)[start];

    return out;
}

size_t str_len(string* in) {
    return in->len;
}

char* str_raw_p(string* in) {
    return in->ptr;
}

void str_free(string* in) {
    free(str_raw_p(in));

    *in = str_alloc(0);
}

