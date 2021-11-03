#include <stddef.h>

#include "errors.h"

#define BUFSIZE 2048

gt_error prun(char *process, int log);

void pstrcat(char*, char*);

void pstrcpy(char*, char*);

void copycat(char*, char*, char*); 

void *smalloc(size_t);

char *copycatalloc(size_t, char*, char*);
