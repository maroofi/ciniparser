#include <stdio.h>
#ifndef _CINI_PARSER_H
#define _CINI_PARSER_H


#define CINI_ERROR_SUCCESS 0
#define CINI_ERROR_FAIL    1
#define CINI_ERROR_MEMORY_ALLOCATION 2
#define CINI_ERROR_KVPAIR_OUT_OF_SECTION 3
#define CINI_ERROR_CAN_NOT_OPEN_CONFIG_FILE 4
#define CINI_ERROR_INVALID_SECTION_NAME 5
#define CINI_ERROR_SECTION_NAME_TOO_LONG 6
#define CINI_ERROR_INVALID_KEY_VALUE_PAIR 7
#define CINI_ERROR_EMPTY_KEY 8

#define CINI_GET_VALUE(C,S,K) cini_get_value(C, S, K)
#define CINI_INIT() cini_init_config()
#define CINI_OK(C) (C && C->err_code == CINI_ERROR_SUCCESS? 1:0)
#define CINI_READ_CONFIG(F,C) cini_parse_from_file(F,C)
#define CINI_PERROR(C) cini_print_error(C->err_code)
#define CINI_FREE(C) cini_free(C)

struct _cini_kvpair{
    char * key;
    char * value;
    struct _cini_kvpair * next;
};

typedef struct _cini_kvpair cini_kvpair;

struct _cini_section{
    char * section_name;
    unsigned int num_of_properties;
    cini_kvpair * kv_list;
    struct _cini_section * next;
};

typedef struct _cini_section cini_section;

struct _cini_config{
    unsigned int err_code;
    cini_section * sections;
};

typedef struct _cini_config cini_config;

void cini_parse_from_file(char * filename, cini_config * config);
void cini_print_error(unsigned short int err_code);
static char * cini_strip_line(char * line, cini_config * config);
static char * _cini_strip_line(char * line, cini_config * config);
static char * cini_readline(FILE * fp, cini_config * config);
static short int cini_is_section_name_valid(char * section_name, cini_config * config);
static cini_section * cini_create_new_section(char * section_name);
static void cini_add_section_to_config(cini_config *, cini_section *);
static cini_kvpair * cini_create_new_kvpair(char *, char *, cini_config *);
static void cini_add_kvpair_to_section(cini_kvpair * kvpair, cini_config * config);
cini_config * cini_init_config(void);
char ** cini_get_all_section_names(cini_config*);
char ** cini_get_all_keys(cini_config * config, char * section);
char * cini_get_value(cini_config * conf, char * section, char * key);
void cini_free(cini_config * config);
static inline int _to_lower(int c);
static inline int _is_al_num(int c);
static size_t _str_len(const char * str);
#endif
