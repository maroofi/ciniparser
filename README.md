# ciniparser
C .INI parser (.ini config file parser)

Small C code to parse .ini configuration files.


##### Why anothre .ini parser?

_I found other .ini parsers very complecated with lots of unnecessary functions and initialization steps. The whole purpose of configuration file is to store some settings to have more flexibility in the code. That's it. It's not a database or your personal diary. If you are looking for a .ini parser which can do a lot of things, you should probably look for the problem somewhere else (e.g., your code)._

_Currently there is no .ini writer in this code. This is just a reader and parser. Writing into an ini file is just as easy as writing files in C._
### Parsing rules

This library only supports the stable features of .ini files ([See here](https://en.wikipedia.org/wiki/INI_file#Stable_features)).

Here is the rules:

1. Each key-value pair **must** be inside a section. So if you want to have some global options, then just define them inside `[global]` section.
2. keys and sections will be all converted to lower-case characters.
3. Whitespaces around the keys and values will be stripped.
4. comments line starts with `;`
5. key-values are in a form of `key = values` (spaces around both keys and values are optional)
6. Sections define with brackets (`[sectionName]` or `[Section Name]` or `[section_name]`)
7. You can not use `[` or `]` as part of a section name
8. You can not use `=` as part of the key name.
9. No inline comment is allowed (e.g., this is wrong `a=b  ;this is a comment`)


### How it works

There are two ways to use the library. 
1. using the low level functions and structures -> see `test_1.c` in `test` directory.
2. using high level macros -> see `test_2.c` in `test` directory.

### Macro definitions

* **CINI_INIT()**: Initialize the config parser module
* **CINI_READ_CONFIG()**: Read the configuration file from the disk and parse it
* **CINI_OK()**: Checks if everything is OK (if there is no error)
* **CINI_PERROR()**: Prints the possible error to stdout
* **CINI_GET_VALUE()**: Reads the value of the specified section/key
* **CINI_FREE()**: free the memory of the config parser

### Errors

* **CINI_ERROR_SUCCESS**: no error
* **CINI_ERROR_FAIL**: failed for unknown reason
* **CINI_ERROR_MEMORY_ALLOCATION**: Memory allocation failed (malloc faild)
* **CINI_ERROR_KVPAIR_OUT_OF_SECTION**: There is a key-value pair outside the section definition
* **CINI_ERROR_CAN_NOT_OPEN_CONFIG_FILE**: The configuration file can not be openned
* **CINI_ERROR_INVALID_SECTION_NAME**: Section name is invalid (probably invalid characters used)
* **CINI_ERROR_SECTION_NAME_TOO_LONG**: Section name is too long (max is 255 characters)
* **CINI_ERROR_INVALID_KEY_VALUE_PAIR**: Key-Value pair is invalid (there is no `=` sign)
* **CINI_ERROR_EMPTY_KEY**: There is no key for a provided value (e.g., `=127.0.0.1`)


