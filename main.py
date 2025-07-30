import yaml
import argparse
import os

class c_header_gen:
    ACCESS_TYPES = ['s', 'u', 'vs', 'vu']
    FOUR_SPACE = (' ' * 4)

    def __init__(self, data):
        self.var_name = self.safe_format_name(data['name'])
        self.schema = '"\\"' + data.get("$schema", "") + '\\""'
        self.kind = self.to_c_str(data.get("kind", ""))
        self.name = self.to_c_str(data.get("name", ""))
        self.long_name = self.to_c_str(data.get("long_name", ""))
        self.description = self.to_c_str(data.get("description", ""))
        self.assembly = self.to_c_str(data.get("assembly", ""))
        self.operation = self.to_c_str(data.get("operation()", ""))
        self.sail = self.to_c_str(data.get("sail()", ""))
        self.data_indep_timing = int(data.get("data_independent_timing", -1))

        # 'definedBy' data
        defined_by_data = data.get("definedBy", tuple())
        if not defined_by_data:
            self.defined_by = (self.to_c_str("anyOf"), [])
        else:
            k = next(iter(defined_by_data))
            self.defined_by = (
                self.to_c_str(k), # mode
                [self.to_c_str(v) for v in defined_by_data[k]] # extensions list
            )

        # 'access' data
        self.access = data.get("access", {})
        for k in self.ACCESS_TYPES:
            self.access[k] = self.to_c_str(self.access.setdefault(k, ""))

        self.encoding = data.get("encoding", {})
        self.match = self.to_c_str(self.encoding.get("match", ""))
        self.variables = self.encoding.get("variables", [])

    def to_c_str(self, val):
        if isinstance(val, str):
            return '"' + val.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'
        return '""'

    # parse encoding variable in C struct format
    def parse_encoding_var(self, var):
        name = self.to_c_str(var.get("name", ""))
        location = self.to_c_str(str(var.get("location", "")))
        not_val = var.get("not", -1)

        return self.FOUR_SPACE + f'{{ {name}, {location}, {not_val} }}'

    def safe_format_name(self, name):
        return name.replace('.', '_')

    def emit(self, output_filename):
        name_upper = self.var_name.upper()

        # encoding variables
        vars_str = ",\n".join([self.parse_encoding_var(v) for v in self.variables])
        # definedBy extensions
        exts_str = ", ".join(self.defined_by[1])

        header_str = f"""#ifndef INST_{name_upper}_H
#define INST_{name_upper}_H

#include "inst_types.h"

static inst_encoding_var_t {self.var_name}_encoding_vars[] = {{
{vars_str}
}};

static const char* {self.var_name}_defined_by_exts[] = {{
    {exts_str}
}};

const inst_t {self.var_name}_inst = {{
    .schema_version = {self.schema},
    .kind = {self.kind},
    .name = {self.name},
    .long_name = {self.long_name},
    .description = {self.description},
    .defined_by = {{
        .mode = {self.defined_by[0]},
        .extensions = {self.var_name}_defined_by_exts,
        .ext_count = ARRAY_COUNT({self.var_name}_defined_by_exts)
    }},
    .assembly = {self.assembly},
    .encoding = {{
        .match = {self.match},
        .variables = {self.var_name}_encoding_vars,
        .var_count = ARRAY_COUNT({self.var_name}_encoding_vars)
    }},
    .access = {{
        .s = {self.access["s"]},
        .u = {self.access["u"]},
        .vs = {self.access["vs"]},
        .vu = {self.access["vu"]},
    }},
    .data_independent_timing = {self.data_indep_timing},
    .operation = {self.operation},
    .sail = {self.sail}
}};

#endif /* INST_{name_upper}_H */
"""

        with open(output_filename, "w") as f:
            f.write(header_str)

        print(f"Generated header: {output_filename}")

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("infile", help="YAML input file path (ex. c.add.yaml)")
    parser.add_argument("-o", "--output", help="C header output file", default=None)

    return parser.parse_args()

def main():
    args = parse_args()
    data = yaml.safe_load(open(args.infile))

    out_basename = os.path.splitext(os.path.basename(args.infile))[0]
    out_filename = args.output or f"{out_basename}.h"

    generator = c_header_gen(data)
    generator.emit(out_filename)

if __name__ == "__main__":
    main()
