#include <ciniparser.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char ** argv){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <ini-file>\n", argv[0]);
        return 1;
    }
    cini_config * conf = CINI_INIT();
    if (! CINI_OK(conf)){
        fprintf(stderr, "Can not initialize the config parser...\n");
        return 1;
    }
    CINI_READ_CONFIG(argv[1], conf);
    if (! CINI_OK(conf)){
        CINI_PERROR(conf);
        return 1;
    }
    // assuming we know what we have in the config file
    fprintf(stdout, "%s: %s->%s\n", "owner", "name", CINI_GET_VALUE(conf, "owner", "name"));
    fprintf(stdout, "%s: %s->%s\n", "owner", "lastname", CINI_GET_VALUE(conf, "owner", "lastname"));
    fprintf(stdout, "%s: %s->%s\n", "owner", "non-existent-key", CINI_GET_VALUE(conf, "owner", "non-existent-key"));

    CINI_FREE(conf);
    return 0;
}
