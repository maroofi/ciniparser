# ciniparser
C .INI parser (.ini config file parser)

Small C code to parse .ini configuration files.

### Parsing rules

This library only supports the stable features of .ini files ([See here](https://en.wikipedia.org/wiki/INI_file#Stable_features)).

Here is the rules:

1. Each key-value pair **must** be inside a section. So if you want to have some global options, then just define them as `[global]` section.
2. keys and sections will be all converted to lower-case characters.
3. Whitespaces around the keys will be stripped.
4. comments line starts with `;`
5. key-values are in a form of `key = values` (spaces around both keys and values are optional)
6. Sections define with brackets (`[sectionName]`)
7. You can not use `[` or `]` as part of a section name
8. You can not use `=` as part of the key name.


### How it works

There are two ways to use the library. 
1. using the low level functions and structures -> see `test_1.c` in `test` directory.
2. using high level macros -> see `test_2.c` in `test` directory.


