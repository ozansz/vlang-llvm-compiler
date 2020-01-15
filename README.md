# Sazak's VLang Compiler

This is a minimal code generator written using Flex, Bison and LLVM.

See [Cem Bozşahin's repo](https://github.com/bozsahin/ceng444/blob/master/project-material/vspecs-2019.pdf) for VLang specs. 

## Usage

To compile the compiler, just run make:

```bash
make
```

It will generate **parser**, **irgen** and **compiler** executables.

### Tools

```bash
cat source_code_file.v | ./parser
cat source_code_file.v | ./irgen
cat source_code_file.v | ./compiler
```

The compiler generates a LLVM IR code to file **out.ll**