#include "cunittest.h"
#include <ciniparser.h>
#include <stdio.h>


int test1(char * filename){
    ABORT_ON_FAIL(1);
    cini_config * conf = CINI_INIT();
    if (! CINI_OK(conf)){
        fprintf(stderr, "Can not initialize the config parser...\n");
        return 1;
    }
    CINI_READ_CONFIG(filename, conf);
    if (! CINI_OK(conf)){
        CINI_PERROR(conf);
        return 1;
    }
    // assuming we know what we have in the config file
    ASSERT_EQ_STR(CINI_GET_VALUE(conf, "owner", "name"), "John");
    ASSERT_EQ_STR(CINI_GET_VALUE(conf, "owner", "lastname"), "Doe");
    ASSERT_EQ_STR(CINI_GET_VALUE(conf, "owner", "organization"), "Acme Widgets Inc.");
    ASSERT_EQ_STR(CINI_GET_VALUE(conf, "database", "server"), "192.0.2.62");
    ASSERT_EQ_STR(CINI_GET_VALUE(conf, "database", "port"), "143");
    ASSERT_EQ_STR(CINI_GET_VALUE(conf, "database", "file"), "\"payroll.dat\"");

    //what if there is a key which doesn't exist?
    ASSERT_NULL(CINI_GET_VALUE(conf, "nosection", "no-key"));
    
    CINI_FREE(conf);
    return 0;
}

int test2(char * filename){
    for (unsigned int i = 0; i< 1000000; ++i)
        test1(filename);
    return 0;
}


int main(int argc, char ** argv){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <config.ini>\n", argv[0]);
        return 1;
    }
    test1(argv[1]);
    test2(argv[1]);
    return 0;
}


