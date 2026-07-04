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
- For now, there are two valid aproach for defining constants, by #define and by enums, most of the project already uses the first method, but in the future we must adopt using enums for better maintenance.

## Name conventions

- Camel case only on module variable and function names, example: counterIndex, matchAccomulator, etc.
- Snake case only used on the prefix on the variable/function module name part only when it's exportable/global, indicating the namespace. For example: ed_matchAccomulator., vid_bufferIndex, etc.
- Private functions must use the '_' prefix, also, they must be strictly used in-module only.
- Constants and #define always in uppercase, also must follow the module prefix rule, however fully in Snake Case. For example: ED_MAX_BUFFER_LENGTH

## Files

- Files/Paths names should be all lowercase, this is for compatibility between linux and DOS.
- Files/Path names length should not exceed 8 characters, this is for compatibility between linux and DOS.

## Dependencies (DEPS folder)

If a functionality is independent of platform and fully portable, then it be reused in the future by moving it to the "deps" folder, this will be a contribution to my personal C framework. If the domain is not generic, then it must stay in the "core" folder. 

Also, all deps should ideally be standalone, this means that could be tested independently. However, there are some cases that some have cross-references, this could be problematic as maintanace
gets complex, so avoid doing this. 

- The #ifdef STANDALONE specifies the entry main function and the test code.
- The dep module should have generic types, avoid using such as bool that could be cross referenced.
- Each dep must be written portable as possible.

NOTE: We need to improve the dependency management so each one are fully isolated from the rest, avoiding cross-references.