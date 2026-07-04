---
trigger: always_on
---

This rule defines the code style for my C projects.

- C89 Standard: This project is aimed for a 90's style programming which in that era evrything was following the C89 standard.

- Function naming scheme by module: Function names has a specific structure 'moduleabreviation_functionName' where the name is in camelCase,

For example: If the function resides in the 'VIDEO' module, then the function name will be "vid_putPixel".

- Early error validation for aviding heavility nested if blocks, for example: if (! videoMemory) return 0;

- Code per lines should not exceed 80 columns.

- Functions calls with more than 3 arguments, each one must be on it's own line.

- Variable always declared on top on the function, ordered by type hierarchy starting by primitives, each type along with the unsigned types, such as int, unsigned int, char, unsigned char. Then bool and Structs and typedef'd stucts at the end.

- Following the previous rule, pointer types should always be NULL-intialized on the top of the function.

## Name conventions

- Camel case only on module variable and function names, example: counterIndex, matchAccomulator, etc.

- Snake case only used on the prefix on the variable/function module name part only when it's exportable/global. For example: ed_matchAccomulator., vid_bufferIndex, etc.

- Private functions must use the '_' prefix, also, they must be strictly used inside the module only.

## Files

- Files/Paths names should be all lowercase, this is for compatibility between linux and DOS.

- Files/Path names length should not exceed 8 characters, this is for compatibility between linux and 
DOS.

