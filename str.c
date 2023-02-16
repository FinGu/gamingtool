#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    size_t ns = nsize;

    char* reptr = (char*)realloc(str_raw_p(in), sizeof(char) * ns);

    OUTMEM(reptr); //free(in->ptr);

    in->size = ns;
    in->ptr = reptr;
}

void str_append_s(string* in, string to_append) {
    long long length_of_the_original_str = in->len,
        length_to_be_appended = to_append.len,
        size_of_the_original_str = in->size,
        available_space = size_of_the_original_str - length_of_the_original_str - __TERM,
        diff;

    if (length_to_be_appended > available_space) {
        diff = length_to_be_appended - available_space;

        __str_realloc(in, size_of_the_original_str + diff);
    }

    memcpy(str_raw_p(in) + length_of_the_original_str, str_raw_p(&to_append), length_to_be_appended + __TERM);

    in->len += length_to_be_appended;
}

void str_append_p(string* in, size_t len, char* to_append) {
    string _tmpb = { len, 0, to_append }; // size=0

    str_append_s(in, _tmpb);
}

void str_clear(string* in, size_t num) {
    char* ptr = &str_raw_p(in)[in->len - num];

    memset(ptr, 0, num);

    in->len -= num;
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

