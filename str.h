#include <stddef.h>
#include <stdbool.h>

#define __TERM 1

#define OUTMEM(x) if(!x){ puts("Fatal error, the program is out of memory"), exit(0); }

typedef struct {
    size_t len;
    size_t size;
    char *ptr;
} string;

string str_view(size_t, char*); //char* as a string view (immutable)

string str_alloc(size_t); // allocate str

void __str_realloc(string*, size_t); //internal realloc

void str_append(string*, string); //append string to string

void str_append_multiple(string*, size_t, ...); //append multiple strings

string str_sub(string*, int, int); //substr

void str_clear(string*, size_t); //clear a number of chars

size_t str_len(string*); //gets len of str

char *str_raw_p(string*); //char* from str

void str_free(string*); //frees string

