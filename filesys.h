#include "config.h"

#define FILE_PERM (S_IRUSR | S_IWUSR | S_IRGRP)

bool can_access(char*, int);

bool __mkdir(char*);

gt_error read_write_file(int, size_t, char*, char*);

gt_error print_files_in_folder(char*); 

gt_error delete_files_in_folder(char*);

gt_error get_game_folder(string*, string, string);

gt_error read_config(size_t, char*, string);

gt_error get_create_folder(string*); 
