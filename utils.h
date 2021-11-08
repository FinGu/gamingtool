#include "config.h"

#define BUFSIZE 2048

gt_error prun(char*, int log);

void pstrcat(char*, char*);

void pstrcpy(char*, char*);

void copycat(char*, char*, char*); 

void *cmalloc(size_t); //custom malloc

void *smalloc(size_t); //str malloc

char *copycatalloc(size_t, char*, char*);

void cfree(void*); //custom free 
