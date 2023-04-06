#include <ciniparser.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char ** argv){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <ini-file>\n", argv[0]);
        return 1;
    }
    cini_config * conf = cini_init_config();
    if (! conf){
        fprintf(stderr, "Can not initialize the config parser...\n");
        return 1;
    }
    cini_parse_from_file(argv[1], conf);
    if (conf->err_code != CINI_ERROR_SUCCESS){
        cini_print_error(conf->err_code);
        return 1;
    }
    char ** sec_arr = cini_get_all_section_names(conf);
    char ** key_list;
    int i = 0;
    int j = 0;
    while(sec_arr[i]){
        j = 0;
        fprintf(stdout, "{%s}\n", sec_arr[i]);
        key_list = cini_get_all_keys(conf, sec_arr[i]);
        if (!key_list){
            fprintf(stdout, "Can not get the keylist for section: %s\n", sec_arr[i]);
            continue;
        }
        while(key_list[j]){
            fprintf(stdout, "    {%s}->{%s}\n", key_list[j], cini_get_value(conf, sec_arr[i], key_list[j]));
            j++;
        }
        free(key_list);
        i++;
    }
    cini_free(conf);
    free(sec_arr);
    return 0;
}
