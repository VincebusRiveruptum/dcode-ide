---
trigger: always_on
---

This rule defines the code style for my C projects.

- C89 Standard: This project is aimed for a 90's style programming which in that era evrything was following the C89 standard.

- Function naming scheme by module: Function names has a specific structure 'moduleabreviation_functionName' where the name is in camelCase,

For example: If the function resides in the 'VIDEO' module, then the function name will be "vid_putPixel".

- Early error validation for aviding heavility nested if blocks, for example: if (! videoMemory) return 0;

- Lines should not exceed 80 columns.

- Functions calls with more than 3 arguments, each one must be on it's own line.

- Varaiable declared always on top on the function, type hierarchy order starts by primitives, each type along with the unsigned types, such as int, unsigned int, char, unsigned char. Then bool and Structs and typedef'd stucts at the end.

- Following the previous rule, pointer types should always initialized to NULL at the top of the function.

