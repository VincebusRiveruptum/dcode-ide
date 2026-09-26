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

## OBOE avoiding conventions

For avoiding OBOE type errors, we need to do the following:

1. Incrementing for loops always as [0, n): for(i=0; i < n ; i++)
2. Decrementing for loops always as: for(i=n-1 ; i >= 0 ; i--)
3. Length =/= Last index :  Last index is Length - 1;
4. Semi-open range convention [start, end):

'''c
// [0, n) means: 0, 1, 2, ..., n-1
void substring(char *str, int start, int end) {
    // Copy chars from start to end-1
    for (int i = start; i < end; i++) {
        putchar(str[i]);
    }
}
'''

5. String length is always real useful chars count, it does not consider null-terminator, this is the C standard convention, for example, strlen() follows this scheme.

6. Static buffer string length: Strings size is set by a constant which also follow the
rule of useful real characters, however we need to always add null character to the size constant. 

'''c
#define MAX_LINE_LEN 256

typedef struct {
    char buffer[MAX_LINE_LEN + 1];  // +1 for NULL
    int length;  // DOES NOT INCLUDE NULL
} Line;

// Invariant: buffer[length] == '\0' ALWAYS
// buffer[0..length-1] are valid chars.

void append_char(Line *line, char ch) {
    if (line->length < MAX_LINE_LEN) {
        line->buffer[line->length++] = ch;
        line->buffer[line->length] = '\0';  // keep invariant
    }
}

void process_line(Line *line) {
    for (int i = 0; i < line->length; i++) {
        printf("%c", line->buffer[i]);
    }
}
'''

NOTE: We need to improve the dependency management so each one are fully isolated from the rest, avoiding cross-references.