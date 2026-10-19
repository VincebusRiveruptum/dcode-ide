# C89 / ANSI C Conformance & Multi-Target Reference

This guide summarizes standard C89 (ANSI X3.159-1989 / ISO/IEC 9899:1990) requirements and compiler nuances for Watcom DOS (`wcc386`) and Linux GCC (`gcc`).

---

## 1. Core C89 Language Rules

### A. Comments
* **Strict C89**: Only block comments `/* ... */` are permitted.
* **C++ Comments**: Single line `// ...` comments are forbidden and trigger `-pedantic` errors in GCC.
* **Macro expansions**: Never place `//` comments at the end of `#define` directives; in strict C89, the comment may not be stripped and can leak into code expansions.

### B. Variable Declarations
* All variable declarations in a block/scope must appear at the **very beginning of the block**, before any executable statements or logic.
* Mixing declarations and code triggers `warning: ISO C90 forbids mixed declarations and code [-Wdeclaration-after-statement]`.
* **Project Type Hierarchy Rule** (from `.agents/rules/c-code-stlye.md`):
  Order declarations top-to-bottom by:
  1. Signed primitives: `int`, `long`, `short`, `char`
  2. Unsigned primitives: `unsigned int`, `unsigned long`, `unsigned char`
  3. `bool`
  4. Structs / typedef'd structs
  5. Pointers (must be `NULL`-initialized at the top)

### C. Enum Definitions
* In C89, an enumerator list must **not** have a trailing comma before the closing brace `}`.
  ```c
  /* ILLEGAL in C89 (-Wpedantic) */
  enum Color { RED, GREEN, BLUE, };

  /* LEGAL C89 */
  enum Color { RED, GREEN, BLUE };
  ```

### D. Integer Types & 64-bit Integers
* `long long` was introduced in C99 and is not part of C89 (`[-Wlong-long]`).
* Use `long` or `unsigned long` for 32-bit timestamps, byte offsets, or sizes.
* For millisecond timestamps on 32-bit systems, `unsigned long` gives ~49.7 days before rollover, which is standard.

### E. Extended ASCII & Character Literals
* Character literals or byte values > 127 (e.g. CP437 box drawing characters `0xB0`, `0xC9`, `218`) exceed the range of signed 8-bit `char` (`[-128, 127]`).
* Assigning them to `char` triggers `warning: overflow in conversion from 'int' to 'char' changes value [-Woverflow]` and causes unexpected sign extension when bitwise OR'd with 16-bit attributes.
* **Fix**: Use `unsigned char` for CP437 bytes, box characters, and buffer cells.

### F. End-of-File Newline
* ISO C requires every source file (`.c` and `.h`) to end with a newline `\n`.
* OpenWatcom emits `Warning! W138: No newline at end of file` if this is violated.

---

## 2. Compiler-Specific Flags and Extensions

### A. Linux (GCC)
* Flags: `-std=c89 -pedantic -Wall -Wextra`
* Feature test macros:
  Standard C89 glibc headers hide POSIX functions (`strdup`, `snprintf`, `usleep`, `fileno`) when `-std=c89` is enabled.
  To expose standard POSIX/UNIX facilities portably, define before includes:
  ```c
  #ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE
  #endif
  #ifndef _POSIX_C_SOURCE
  #define _POSIX_C_SOURCE 200809L
  #endif
  ```

### B. Watcom C / OpenWatcom (`wcc386`)
* Strict ANSI Flag: `-za` (disables OpenWatcom language extensions and non-ANSI keywords).
* Warning Level Flag: `-w4` (highest warning level).
* **Inline Assembly (`#pragma aux`) under `-za`**:
  Under `-za`, auxiliary tokens (`parm`, `value`, `modify`) and register names (`ax`, `dx`, etc.) are non-ANSI extensions. OpenWatcom requires ISO-reserved identifiers with double underscores:
  ```c
  void outPortb(int, unsigned char);

  #pragma aux outPortb = \
      "out dx, al"       \
      __parm [__dx] [__al]

  void _set80x25_asm(void);

  #pragma aux _set80x25_asm = \
      "mov ax, 0x1202"        \
      "mov bl, 0x30"          \
      "int 0x10"              \
      __modify [__ax __bx __cx __dx]
  ```
* Every assembly helper must have a corresponding prototype declared before calling it, to avoid `Warning! W131: No prototype found`.
* Exposing process spawning functions in `<process.h>`:
  Watcom guards `spawnl` and `P_WAIT` behind `!defined(_NO_EXT_KEYS)`. Under `-za`, undefine it before `#include <process.h>`:
  ```c
  #undef _NO_EXT_KEYS
  #include <process.h>
  ```
