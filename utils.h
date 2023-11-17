#include "config.h"

#define BUFSIZE 1024 

typedef struct {
    size_t size;
    char **ptr;
} __split_out;

__split_out split(char, string);

void free_split(__split_out);

string get_file_from_path(string); 

gt_error prun(char*, struct __args*, struct __args*, struct __args*, char*, bool); 
