#ifndef INST_TYPES_H
#define INST_TYPES_H

#include <stdint.h>

#define ARRAY_COUNT(arr) (sizeof((arr)) / sizeof((arr)[0]))

// if empty string - don't output field
// int not specd = -1

typedef struct {
    const char* s; // always | sometimes | never
    const char* u;
    const char* vs;
    const char* vu;
} access_t;

typedef struct {
    const char* name;
    const char* location;
    int not_val;
} inst_encoding_var_t;

typedef struct {
    const char* match;
    inst_encoding_var_t* variables;
    size_t var_count;
} inst_encoding_t;

typedef struct {
    const char* mode; // anyOf or oneOf
    const char** extensions;
    size_t ext_count;
} inst_defined_by_t;

typedef struct {
    const char* schema_version;
    const char* kind;
    const char* name;
    const char* long_name;
    const char* description;
    inst_defined_by_t defined_by;
    const char* assembly;
    inst_encoding_t encoding;
    access_t access;
    int data_independent_timing;
    const char* operation;
    const char* sail;
} inst_t;

#endif /* INST_TYPES_H */
