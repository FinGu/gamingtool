#ifndef GT_ERR
#define GT_ERR

typedef enum {
    ok = 0,
    //main
    invalid_input,
    //file reading starts
    failed_to_open,
    failed_to_read,
    failed_to_write,
    failed_to_delete,
    failed_to_create_dir,
    failed_to_execute,
    home_not_found,
    //file reading ends
    //parsing starts
    failed_to_parse,
    cjson_failure,
    //parsing ends
    //run stats
    game_not_found,
    couldnt_find_wine,
    failed_to_start,
    //run ends
    //create starts
    game_already_exists,
    wine_not_found,
    //create ends
    //install starts
    curl_bad_instance,
    target_not_found,
    target_already_exists,
    //install ends

} gt_error;

void print_error(gt_error);

#endif
