#include <stdio.h>
#include "inst_types.h"
#include "c.add.h"

#define TWO_SPACE   "  "
#define FOUR_SPACE  "    "
#define SIX_SPACE   "      "
#define CHECK_STR_NOT_EMPTY(str) ((str) && (str)[0])

extern const inst_t c_add_inst;

void write_string(FILE* out, const char* str, int is_last_line) {
    while (*str) {
        fprintf(out, TWO_SPACE);
        while (*str && *str != '\n') fputc(*str++, out);
        if (*str == '\n') { fputc('\n', out); str++; }
    }

    // if not last line and no newline at the end of the string
    if (!is_last_line && *(str - 1) != '\n')
        fprintf(out, "\n");
}

void emit_yaml(FILE* out, const inst_t* instruction) {
    int is_last_line = 0; // keep track whether it is the last line

    fprintf(out, "$schema: %s\n", instruction->schema_version);
    fprintf(out, "kind: %s\n", instruction->kind);
    fprintf(out, "name: %s\n", instruction->name);
    fprintf(out, "long_name: %s\n", instruction->long_name);

    if (CHECK_STR_NOT_EMPTY(instruction->description)) {
        fprintf(out, "description: |\n");
        write_string(out, instruction->description, is_last_line);
    }

    // 'definedBy' data
    fprintf(out, "definedBy:\n");
    fprintf(out, TWO_SPACE "%s:\n", instruction->defined_by.mode);

    for(size_t i = 0; i < instruction->defined_by.ext_count; i++)
        fprintf(out, FOUR_SPACE "- %s\n", instruction->defined_by.extensions[i]);

    fprintf(out, "assembly: %s\n", instruction->assembly);

    // 'encoding' data
    fprintf(out, "encoding:\n");
    fprintf(out, TWO_SPACE "match: %s\n", instruction->encoding.match);

    if (instruction->encoding.var_count > 0) {
        fprintf(out, TWO_SPACE "variables:\n");

        for (size_t i = 0; i < instruction->encoding.var_count; i++) {
            inst_encoding_var_t v = instruction->encoding.variables[i];
            fprintf(out, FOUR_SPACE "- name: %s\n", v.name);
            fprintf(out, SIX_SPACE "location: %s\n", v.location);

            if (v.not_val != -1) 
                fprintf(out, SIX_SPACE "not: %d\n", v.not_val);
        }
    }

    // 'access' data
    fprintf(out, "access:\n");
    fprintf(out, TWO_SPACE "s: %s\n", instruction->access.s);
    fprintf(out, TWO_SPACE "u: %s\n", instruction->access.u);
    fprintf(out, TWO_SPACE "vs: %s\n", instruction->access.vs);
    fprintf(out, TWO_SPACE "vu: %s\n", instruction->access.vu);
    
    if (instruction->data_independent_timing != -1) {
        fprintf(out, "data_independent_timing: %s\n",
                instruction->data_independent_timing ? "true" : "false");
    }

    if (CHECK_STR_NOT_EMPTY(instruction->operation)) {
        fprintf(out, "operation(): |\n");
        write_string(out, instruction->operation, is_last_line);
    }

    if (CHECK_STR_NOT_EMPTY(instruction->sail)) {
        fprintf(out, "sail(): |\n");
        is_last_line = 1;
        write_string(out, instruction->sail, is_last_line);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Required output filename parameter\n");
        return 1;
    }

    const char* fname = argv[1]; 
    FILE* out = fopen(fname, "w");

    if (!out) {
        fprintf(stderr, "Failed to open output file\n");
        return 1;
    }

    emit_yaml(out, &c_add_inst);
    printf("Wrote to %s\n", fname);

    fclose(out);
    return 0;
}
