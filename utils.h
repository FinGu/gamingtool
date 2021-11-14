#include "config.h"

#define BUFSIZE 2048

string get_file_from_path(string); 

void escapeshellargs(string*, string); 

gt_error prun(char*, int log);

void pstrcat(char*, char*);

void pstrcpy(char*, char*);

void copycat(char*, char*, char*); 

void *scalloc(size_t, size_t); //safe calloc

string salloc(size_t); //string alloc

void sfree(string); //string free

char *copycatalloc(size_t, char*, char*);
