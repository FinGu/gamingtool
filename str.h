#include <stddef.h>
#include <stdbool.h>

#define __TERM 1

#define OUTMEM(x) if(!x){ puts("Fatal error, the program is out of memory"), exit(0); }

typedef struct {
    size_t len;
    size_t size;
    bool flag;
    char *ptr;
} string;

string str_alloc(size_t);

void str_realloc(string*, size_t);

void str_append_p(string*, size_t, char *);

void str_append_s(string*, string);

void str_clear(string*, size_t);

size_t str_len(string*);

char *str_raw_p(string*);

void str_free(string*);

