---
trigger: always_on
---

This rule defines the code style for my C projects. 

- C89 Standard: This project is aimed for a 90's style programming which in that era evrything was following the C89 standard.
- Function naming scheme by module: Function names has a specific structure 'moduleabreviation_functionName' where the name is in camelCase,
for example: If the function resides in the 'VIDEO' module, then the function name will be "vid_putPixel".
- Early error validation for aviding heavility nested if blocks, for example: if (! videoMemory) return 0;
