#ifndef GT_ERR
#define GT_ERR

typedef enum {
    ok = 0,
    //file reading starts
    failed_to_open,
    failed_to_read,
    failed_to_create_dir,
    home_not_found,
    //file reading ends
    //parsing starts
    failed_to_parse,
    //parsing ends
    //run stats
    couldnt_find_wine,
    couldnt_find_script,
    game_not_found,
    failed_to_start,
    //run ends

} gt_error;

void print_error(gt_error);

#endif
