#include <ciniparser.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Returns all keys for the specified section
 *
 */
char ** cini_get_all_keys(cini_config * config, char * section){
    if (NULL == section)
        return NULL;
    cini_config * tmp = config;
    if (tmp == NULL)
        return NULL;
    cini_kvpair *tmp_kv = NULL;
    if (_str_len(section) > 255){
        return NULL;
    }
    char new_sec[256] = {0};
    strcpy(new_sec, section);
    for(int i=0; i< _str_len(section); i++)
        new_sec[i] = _to_lower(new_sec[i]);
    cini_section * seclist = tmp->sections;
    while (seclist){
        if (strcmp(new_sec, seclist->section_name) == 0)
            break;
        seclist = seclist->next;
    }
    if (NULL == seclist)
        return NULL;
    char ** str = (char**) malloc(sizeof(char*) * (seclist->num_of_properties + 1));
    if (!str)
        return NULL;
    tmp_kv = seclist->kv_list;
    for (unsigned int i=0; i< seclist->num_of_properties; ++i){
        str[i] = tmp_kv->key;
        tmp_kv = tmp_kv->next;
    }
    str[seclist->num_of_properties] = NULL;
    return str;
}


void cini_free(cini_config * config){
    if (!config)
        return;
    cini_section * sec = config->sections;
    if (!sec){
        free(config);
        return;
    }
    cini_kvpair * tmp_kv;
    cini_section * tmp_sec;
    cini_kvpair * temp;
    while (sec){
        tmp_kv = sec->kv_list;
        for (unsigned int i=0; i< sec->num_of_properties; ++i){
            if (tmp_kv){
#ifdef DEBUG
                fprintf(stdout, "Let's free: %s\n", tmp_kv->key);
#endif
                free(tmp_kv->key);
                free(tmp_kv->value);
                temp = tmp_kv->next;
                free(tmp_kv);
                tmp_kv = temp;
                continue;
            }
        }
#ifdef DEBUG
        fprintf(stdout, "Let's free Section: %s\n", sec->section_name);
#endif
        tmp_sec = sec->next;
        free(sec->section_name);
        free(sec);
        sec = tmp_sec;
    }
    free(config);
}



char * cini_get_value(cini_config * conf, char * section, char * key){
    if (conf == NULL || section == NULL || key == NULL)
        return NULL;
    cini_config * tmp = conf;
    if (_str_len(section) > 256)
        return NULL;
    char new_sec[256] = {0};
    strcpy(new_sec, section);
    for (int i=0; i< _str_len(section); i++)
        new_sec[i] = _to_lower(new_sec[i]);
    cini_section * seclist = tmp->sections;
    while(seclist){
        if (strcmp(new_sec, seclist->section_name) == 0)
            break;
        seclist = seclist->next;
    }
    if (NULL == seclist)
        return NULL;
    cini_kvpair *tmp_kv = seclist->kv_list;
    if (_str_len(key) > 255)
        return NULL;
    char new_key[256] = {0};
    strcpy(new_key, key);
    for (unsigned int i=0; i< _str_len(new_key); ++i)
        new_key[i] = _to_lower(new_key[i]);
    for (unsigned int i=0; i< seclist->num_of_properties; ++i){
        if (strcmp(new_key, tmp_kv->key) == 0){
            return tmp_kv->value;
        }
        tmp_kv = tmp_kv->next;
    }
    return NULL;
}


char ** cini_get_all_section_names(cini_config * config){
    unsigned int count = 0;
    cini_config * tmp = config;
    if (tmp == NULL){
        return NULL;
    }
    cini_section * sec = tmp->sections;
    if (sec == NULL){
        return NULL;
    }
    while (sec){
        count++;
        sec = sec->next;
    }
#ifdef DEBUG
    fprintf(stdout, "DEBUG: number of sections: %d\n", count);
#endif
    char ** sec_arr = (char**) malloc(sizeof(char*) * (count+1));
    if (sec_arr == NULL){
        return NULL;
    }
    sec = tmp->sections;
    for (unsigned int i=0; i< count; ++i){
        sec_arr[i] = sec->section_name;
        sec= sec->next;
    }
    sec_arr[count] = (char *) 0x00;
    return sec_arr;
}


cini_config * cini_init_config(void){
    cini_config * conf = (cini_config*) malloc(sizeof(cini_config));
    if (NULL == conf){
        return NULL;
    }
    conf->err_code = CINI_ERROR_SUCCESS;
    conf->sections = NULL;
    return conf;
}

/**
 * @brief Parses ini configuration from a file
 *
 */
void cini_parse_from_file(char * filename, cini_config * config){
    // reads a file line by line and parse it
    FILE * fp = fopen(filename, "r");
    if (NULL == fp){
        config->err_code = CINI_ERROR_CAN_NOT_OPEN_CONFIG_FILE;
        return;
    }
    char * line = NULL;
    char * new_line = NULL;
    char  * sec_name = NULL;
    while ((line = cini_readline(fp, config)) != NULL){
        // read line by line
        new_line = cini_strip_line(line, config);
        if (!new_line || config->err_code != CINI_ERROR_SUCCESS){
            free(line);
            fclose(fp);
            return;
        }
        free(line);
        if (_str_len(new_line) == 0){
            free(new_line);
            continue;
        }
        // eat up comment line starts with ;
        if (new_line[0] == ';'){
            free(new_line);
            continue;
        }
        // if line starts with [, we have a new section
        if (new_line[0] == '['){
            if (cini_is_section_name_valid(new_line, config) == 0){
                //wrong section name
                free(new_line);
                fclose(fp);
                return;
            }
            //nice section name
            sec_name = (char*) malloc(_str_len(new_line) -2 +1);
            if (!sec_name){
                free(new_line);
                fclose(fp);
                config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
                return;
            }
            strncpy(sec_name, new_line + 1, _str_len(new_line) -2);
            sec_name[_str_len(new_line) -2] = '\0';
            cini_section * new_section = cini_create_new_section(sec_name);
            free(sec_name);
            if (new_section == NULL){
                config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
                fclose(fp);
                free(new_line);
                return;
            }
            cini_add_section_to_config(config, new_section);
            free(new_line);
            continue;
        }
        // if line starts with whatever else, it's a key-value pair
        line = strstr(new_line, (const char*) "=");
        if (NULL == line){
            config->err_code = CINI_ERROR_INVALID_KEY_VALUE_PAIR;
            fclose(fp);
            free(new_line);
            return;
        }
        cini_kvpair * kvpair = cini_create_new_kvpair(new_line, line, config);
        if (!kvpair){
            fclose(fp);
            free(new_line);
            return;
        }
        // append the new kvpair to the latest section
        cini_add_kvpair_to_section(kvpair, config);
        if (config->err_code != CINI_ERROR_SUCCESS){
            fclose(fp);
            free(new_line);
            return;
        }
        free(new_line);
    }
    fclose(fp);
}


static void cini_add_kvpair_to_section(cini_kvpair * kvpair, cini_config * config){
    cini_section * tmp = config->sections;
    if (tmp == NULL){
        // we still don't have section but trying to add kv-pair
        config->err_code = CINI_ERROR_KVPAIR_OUT_OF_SECTION;
        return;
    }
    while (tmp->next)
        tmp = tmp->next;
    cini_kvpair * tmp_kv = tmp->kv_list;
    if (tmp_kv == NULL){
        tmp->kv_list = kvpair;
        tmp->num_of_properties += 1;
        return;
    }
    while (tmp_kv->next)
        tmp_kv = tmp_kv->next;
#ifdef DEBUG
    fprintf(stdout, "Adding %s->%s to config\n", kvpair->key, kvpair->value);
#endif
    tmp_kv->next = kvpair;
    tmp->num_of_properties += 1;
}


static cini_kvpair * cini_create_new_kvpair(char * line, char * equal_pos, cini_config * config){
    // error: on empty key
    // lowercase the key
    // no error for empty value
    // strip both key and value
    char * tmp = line;
    if (line[0] == '='){
        config->err_code = CINI_ERROR_EMPTY_KEY;
        return NULL;
    }
    for (;*tmp && tmp < equal_pos; tmp++){*tmp = _to_lower(*tmp);}
    char * key = (char*) malloc(equal_pos - line + 1);
    if (!key){
        config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }
    strncpy(key, line, equal_pos - line);
    key[equal_pos - line] = '\0';
    //test key
    char * new_key = cini_strip_line(key, config);
    free(key);
#ifdef DEBUG
    fprintf(stdout, "new_key is: '%s'\n", new_key);
#endif
    if (!new_key || config->err_code != CINI_ERROR_SUCCESS){
        return NULL;
    }
    char * value = (char*) malloc(_str_len(equal_pos)+1);
    if (!value){
        free(new_key);
        config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }
    value[_str_len(equal_pos)] = '\0';
    strcpy(value, equal_pos+1);
    char * new_value = cini_strip_line(value, config);
    if (!new_value || config->err_code != CINI_ERROR_SUCCESS){
        free(value);
        free(new_key);
        return NULL;
    }
    free(value);
    cini_kvpair * kvpair = (cini_kvpair*) malloc(sizeof(cini_kvpair));
    if (!kvpair){
        free(new_value);
        free(new_key);
        config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }
    kvpair->key = new_key;
    kvpair->value = new_value;
    kvpair->next = NULL;
    return kvpair;
}


static void cini_add_section_to_config(cini_config* config, cini_section * section){
    cini_section * tmp = config->sections;
    if (NULL == tmp){
        config->sections = section;
        return;
    }
    while (tmp && tmp->next)
        tmp = tmp->next;
    tmp->next = section;
    return;
}



static cini_section * cini_create_new_section(char * section_name){
    cini_section * new_section = (cini_section *) malloc(sizeof(cini_section));
    if (!new_section)
        return NULL;
    new_section->section_name = (char *) malloc(_str_len(section_name) + 1);
    char * tmp = section_name;
    for(; *tmp;++tmp){*tmp = _to_lower(*tmp);}
    strcpy(new_section->section_name, section_name);
    new_section->num_of_properties = 0;
    new_section->kv_list = NULL;
    new_section->next = NULL;
    return new_section;
}

/**
 * @brief Checks if the section name is valid or not also sets the error message
 * @param section_name Section name string
 * @return 1 if it's valid or 0 if it's not valid.
 */
static short int cini_is_section_name_valid(char * section_name, cini_config * config){
    // section name always starts with [ and ends with ]
#ifdef DEBUG
        fprintf(stdout, "Section name: '%s', len: %d\n", section_name, _str_len(section_name));
#endif
    if (section_name[0] != '[' || section_name[_str_len(section_name) -1] != ']'){
        config->err_code = CINI_ERROR_INVALID_SECTION_NAME;
        return 0;
    }
    // section name smaller than 255
    if (_str_len(section_name) > 255){
        config->err_code = CINI_ERROR_SECTION_NAME_TOO_LONG;
        return 0;
    }
    char name[256] = {0};
    char * tmp = name;
    strncpy(name, section_name + 1, _str_len(section_name) -2);
    int name_invalid = 0;
    int ch;
    config->err_code = CINI_ERROR_SUCCESS;
#ifdef DEBUG
    fprintf(stdout, "tmp: '%s', len: %d\n", tmp, _str_len(tmp));
#endif
    if (config->err_code != CINI_ERROR_SUCCESS)
        return 0;
    //successfull
    return 1;
}


/**
 * @brief Reads a file line by line
 *
 *
 */
static char * cini_readline(FILE * fp, cini_config * config){
    const int MAX_LINE_SIZE = 255;
    char * line = (char*) calloc(MAX_LINE_SIZE + 1, sizeof(char));
    int ch = -1;
    char * new_memory = NULL;
    unsigned long int count = 0;
    while ((ch = fgetc(fp)) != EOF){
        line[count++] = ch;
        if (ch == '\n'){
            return line;
        }
        if (count % MAX_LINE_SIZE == 0){
            // we need to reallocate
            new_memory = (char*)realloc(line, _str_len(line) + MAX_LINE_SIZE);
            if (new_memory == NULL){
                config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
                free(line);
                return NULL;
            }
            line = new_memory;
        }
    }
    if (count == 0){
        // we are at the end of file
        free(line);
        return NULL;
    }
    line[count] = '\0';
    return line;
}

/**
 * @brief Prints an appropriate error message based on the error code
 *
 */
void cini_print_error(unsigned short int err_code){
    if (err_code == CINI_ERROR_MEMORY_ALLOCATION){
        fprintf(stderr, "Can not allocate memory using malloc() function!\n");
        return;
    }
    if (err_code == CINI_ERROR_KVPAIR_OUT_OF_SECTION){
        fprintf(stderr, "There is at least one key-value pair without having a section!\n");
        return;
    }
    if (err_code == CINI_ERROR_INVALID_SECTION_NAME){
        fprintf(stderr, "Section name must start with '[', ends with ']' and only alphabedical characters\n");
        return;
    }
    if (err_code == CINI_ERROR_SECTION_NAME_TOO_LONG){
        fprintf(stderr, "Section name must be smaller than 255 characters\n");
        return;
    }
    if (err_code == CINI_ERROR_EMPTY_KEY){
        fprintf(stderr, "Key can not be empty...\n");
        return;
    }
    if (err_code == CINI_ERROR_CAN_NOT_OPEN_CONFIG_FILE){
        fprintf(stderr, "Can not open the config file...\n");
        return;
    }
    if (err_code == CINI_ERROR_INVALID_KEY_VALUE_PAIR){
        fprintf(stderr, "Invalid key-value pair (must be key=value)\n");
        return;
    }
    return;
}


static char * _cini_strip_line(char * line, cini_config * config){
    char seq[] = " \t\n\r\x0b\x0c";  // all whitespace
    unsigned int i = 0;
    unsigned int j = 0;
    const char * tmp = line;
    char ch;
    unsigned int found = 0;
    while (*tmp != '\0'){
        while((ch = seq[i++]) != '\0'){
            if (ch == *tmp){
                tmp += 1;
                i = 0;
                found = 1;
                break;
            }
            found=0;
        }
        if (!found)
            break;
    }
    //now tmp is the pointer to the new string
    char * new_str = (char *)malloc(sizeof(char) * _str_len(tmp) + 1);
    if (NULL == new_str){
        config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }
    strcpy(new_str, tmp);
    config->err_code = CINI_ERROR_SUCCESS;
    return new_str;
}

/**
 * @brief Internal method to strip a line
 *
 */
static char * cini_strip_line(char * line, cini_config * config){
    //strips a line both from left and right side
    //first pass the string
    char * lstrip = _cini_strip_line(line, config);
    if (config->err_code != CINI_ERROR_SUCCESS){
        return NULL;
    }
    //reverse the string
    size_t len = _str_len(lstrip);
    char * reverse_str = (char *) malloc(len + 1);
    if (NULL == reverse_str){
        config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
        free(lstrip);
        return NULL;
    }
    for (unsigned int i=len;i>0; --i)
        reverse_str[len - i] = lstrip[i-1];
    reverse_str[len] = '\0';
    free(lstrip);
    char * rstrip = _cini_strip_line(reverse_str, config);
    free(reverse_str);
    if (NULL == rstrip || config->err_code != CINI_ERROR_SUCCESS)
        return NULL;
    // reverse it again
    len = _str_len(rstrip);
    reverse_str = (char *) malloc(len + 1);
    if (NULL == reverse_str){
        config->err_code = CINI_ERROR_MEMORY_ALLOCATION;
        free(rstrip);
        return NULL;
    }
    for (unsigned int i=len;i>0; --i)
        reverse_str[len - i] = rstrip[i-1];
    reverse_str[len] = '\0';
    free(rstrip);
    return reverse_str;
}

static inline int _to_lower(int c){
    /* to remove tolower() from ctype.h*/
    return c>=0x41 && c<=0x5A?c+0x20:c;
}

static inline int _is_al_num(int c){
    /* to remove isalnum() from ctype.h*/
    return (c>=0x30 && c<=0x39) || (c >=0x41 && c<= 0x5A) || (c >=0x61 && c<=0x7a)?1:0;
}

static size_t _str_len(const char * str){
    char * tmp = (char*)str;
    for (;*tmp; ++tmp);
    return tmp - str;
}
