#ifndef INST_C_ADD_H
#define INST_C_ADD_H

#include "inst_types.h"

static inst_encoding_var_t c_add_encoding_vars[] = {
    { "xs2", "6-2", 0 },
    { "xd", "11-7", 0 }
};

static const char* c_add_defined_by_exts[] = {
    "C", "Zca"
};

const inst_t c_add_inst = {
    .schema_version = "\"inst_schema.json#\"",
    .kind = "instruction",
    .name = "c.add",
    .long_name = "Add",
    .description = "Add the value in xs2 to xd, and store the result in xd.\nC.ADD expands into `add xd, xd, xs2`.\n",
    .defined_by = {
        .mode = "anyOf",
        .extensions = c_add_defined_by_exts,
        .ext_count = ARRAY_COUNT(c_add_defined_by_exts)
    },
    .assembly = "xd, xs2",
    .encoding = {
        .match = "1001----------10",
        .variables = c_add_encoding_vars,
        .var_count = ARRAY_COUNT(c_add_encoding_vars)
    },
    .access = {
        .s = "always",
        .u = "always",
        .vs = "always",
        .vu = "always",
    },
    .data_independent_timing = -1,
    .operation = "XReg t0 = X[xd];\nXReg t1 = X[xs2];\nX[xd] = t0 + t1;\n",
    .sail = "{\n  let rs1_val = X(rd);\n  let rs2_val = X(rs2);\n  X(rd) = rs1_val + rs2_val;\n  RETIRE_SUCCESS\n}"
};

#endif /* INST_C_ADD_H */
