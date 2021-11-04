#include "errors.h"

#include <stddef.h>

int can_access(char*, int);

gt_error read_write_file(int, size_t, char*, char*);

gt_error print_files_in_folder(char*); 

gt_error get_game_folder(char **, char*, char*);

gt_error read_write_config(int, size_t, char*, char*);

gt_error get_create_folder(char**); 
