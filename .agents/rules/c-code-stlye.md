---
trigger: always_on
---

This rule defines the code style for my C projects.

## Syntax Rules

- C89 Standard: This project is aimed for a 90's style programming which in that era evrything was following the C89 standard.

- Function naming scheme by module: Function names has a specific structure 'moduleabreviation_functionName' where the name is in camelCase,
  for example: If the function resides in the 'VIDEO' module, then the function name will be "vid_putPixel".

- Early error validation for aviding heavility nested if blocks, for example: if (! videoMemory) return 0;

## Project environment

As i mentioned, tt is aimed for a 90s programming era, we are writing 32-bit DOS C code aimed for 386-Pentium era machines.

The code is compiled in a virtual machine in DOSBOX X, it uses WATCOM C Compiler, version 10.6. So , from the current operting
system where the workspace is open, it is unable for compiling directly and i have to it by myself manually in the virtual machine.