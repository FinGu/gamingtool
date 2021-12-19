#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>

#include "utils.h"
#include "filesys.h"

string get_file_from_path(string in){
    size_t i;
    char *ptr = NULL;

    for(i = 0; i < in.len; i++){
        if(in.ptr[i] == '/'){
            ptr = &in.ptr[i+1]; //fine because in.len doesnt include the term
        }
    }

    return (string){strlen(ptr), ptr};
} 

void escapeshellargs(string* out, string in){
    size_t len, i, n, j;
    string buf;

    i = n = 0;

    for(; i < in.len; ++i){
        if(in.ptr[i] == '\''){
            ++n; 
        }
    }

    len = (in.len-n) + n * 4;

    buf = salloc(len);
    
    for(i = j = 0; i < in.len; i++){
        if(in.ptr[i] == '\''){
            buf.ptr[j] = '\'';
            buf.ptr[j+1] = '\\';
            buf.ptr[j+2] = '\'';
            buf.ptr[j+3] = '\'';
            j += 4;
        } else{
            buf.ptr[j] = in.ptr[i];
            ++j;
        }
    }

    *out = buf;
}

gt_error prun(char *process, char *log_file, bool log_to_stdout){
    int fd = -1;
    gt_error err = ok;

    FILE *prfile = popen(process, "r");

    char buf[BUFSIZE] = {0};

    if(!prfile){
        err = failed_to_start;
        goto out;
    }

    if(log_file && (fd = open(log_file, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU)) == -1){
        err = failed_to_open;
        goto out;
    }
    
    
    while(fgets(buf, BUFSIZE, prfile)){
        if(log_to_stdout){
            puts(buf);
        }

        if(fd >= 0){
            write(fd, buf, strlen(buf)); // should be buffered (strlen bad)
        }
    }

    out:
    close(fd);
    pclose(prfile);

    return err;
} 

void *scalloc(size_t num, size_t size){ //safe calloc
    void *out = calloc(num, size);

    if(!out){
        puts(PREFIX"Fatal issue, failed to allocate memory");
        exit(-1);
    }

    return out;
}

string salloc(size_t size){ //string alloc
    char *mem = scalloc(size+1, sizeof(char));

    return (string){size, mem};
}

void sfree(string to_free){
    free(to_free.ptr);
}

void pstrcat(char *dest, char *source){
    size_t len1 = strlen(dest), len2 = strlen(source);

    memcpy(dest + len1, source, len2 + 1);
}

void pstrcpy(char *dest, char *source){
    size_t len = strlen(source);

    memcpy(dest, source, len + 1);
}

void copycat(char *buf, char *copy, char *cat){
    pstrcpy(buf, copy);

    pstrcat(buf, cat);
}

char *copycatalloc(size_t size, char *copy, char *cat){
    char *out = scalloc(size+1, sizeof(char));

    copycat(out, copy, cat); 

    return out;
}
