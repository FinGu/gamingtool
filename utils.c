#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>

#include <unistd.h>
#include <sys/wait.h>

#include "utils.h"
#include "filesys.h"

__split_out split(char delim, string args){ //ghetto, must be a \0 ended string
    int i, n, s, len = str_len(&args); 

    char *cpargs = strdup(str_raw_p(&args)), **out;

    for(i = n = 0; i < args.len; ++i){
        if(cpargs[i] == delim){
            cpargs[i] = '\0';
            ++n;
        }
    }

    s = n+1;

    out = malloc(sizeof(char*) * s);

    out[0] = cpargs;

    for(i = n = 0; i < len-1; ++i){
        if(cpargs[i] == '\0'){
            out[(n++)+1] = &cpargs[i+1]; //trick to avoid allocating for each splitted string
        }
    }

    return (__split_out){s, out};
}

void free_split(__split_out in){
    char **ptr = in.ptr;

    free(ptr[0]);

    free(ptr);
}

string get_file_from_path(string in){
    size_t i, len = str_len(&in);
    char *ptr = NULL;
    string out = str_alloc(0);
    char *tmpp = str_raw_p(&in);

    for(i = 0; i < len; i++){
        if(in.ptr[i] == '/'){
            ptr = &tmpp[i+1]; //fine because in.len doesnt include the term
        }
    }

    out = str_view(strlen(ptr), ptr); //string here is used as a container for a ptr and len, other uses like this can be found on main, parse and run

    return out;
} 

char **args_as_argv(struct __args *args, char* process){
    int i, j;

    i = j = 0;

    int sz = 2 + (args ? args->size : 0);

    char **out = calloc(sz, sizeof(char*));

    if(!out){
        exit(0);
    }

    if(process){
        out[0] = process;
        i = 1;
    }

    for(; j < sz-2; ++j){
        out[i++] = args->ptr[j];
    }

    out[sz-1] = NULL;

    return out;
}

gt_error prun(char *process, struct __args *args, struct __args *env, char *log_file, bool log_to_stdout) {
    int sz, last;
    char **inargs, buf[BUFSIZE] = {0};
    gt_error err = ok;

    int fd = 0, spipe[2];
    pid_t pid;

    inargs = args_as_argv(args, process);
    //inenv = args_as_argv(env, NULL);

    //{"echo", "hello", ..., NULL}

    if(pipe(spipe) == -1 || (pid = vfork()) == -1){
        err = failed_to_start;
        goto out;
    }

    if(pid == 0){
        close(spipe[0]);

        dup2(spipe[1], STDOUT_FILENO);

        for(sz = 0; sz < env->size; ++sz){
            putenv(env->ptr[sz]);
        }

        execvp(inargs[0], inargs);

        _exit(0);
    } else{
        close(spipe[1]);

        fd = ((log_file) ? open(log_file, O_CREAT | O_APPEND | O_WRONLY, FILE_PERM) : fd);

        if(fd == -1){
            err = failed_to_open;
            goto out;
        } 

        sz = last = 0;

        while(true){
            memset(buf, 0, (last > sz) ? last : sz);

            last = sz;

            if((sz = read(spipe[0], buf, BUFSIZE)) <= 0){
                break;
            }

            buf[sz-1] = '\n'; //sz = strlen(buf)
            buf[sz] = '\0';

            if(log_to_stdout){
                fputs(buf, stdout);
            }

            if(log_file){
                write(fd, buf, sz); // should be buffered
            }
        }

        close(fd);
    }

    out:
    free(inargs);

    return err;
}
