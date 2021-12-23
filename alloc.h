#include "config.h"

void *scalloc(size_t, size_t); //safe calloc

string salloc(size_t); //string alloc

void sfree(string); //string free

char *copycatalloc(size_t, char*, char*);
