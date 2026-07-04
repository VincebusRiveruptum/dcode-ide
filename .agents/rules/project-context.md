---
trigger: always_on
---

# DCODE IDE

This is a text editor aimed for C programming, targeted for 32-bit DOS and Linux environments. It began due to the necessity of programming DOS projects on my old 486 PC without having to use VS Code from a separate modern machine, or Borland C which I'm not used to working with, so I decided to build my own editor that would feel like VS Code.

This app could be helpful for anyone in my scenario, where there is interest in retro-programming on real hardware but lacking tools for productive work.

## Project Goals

- **Simplicity**: This project aims to go straight to the point, being a simple and powerful editor capable of maximizing productivity for the programmer in old text-based or terminal environments such as DOS or Linux.
- **Portability**: This app initially began as a 32-bit DOS IDE for retro-programming; however, I plan to make it fully portable in the future so it can be used for modern programming on different environments.
- **Performance**: Aimed at 386+ machines. Because it is being developed in C, one of its major advantages is performance; however, it also aims to use as few resources as possible.
- **Efficiency and Ease of Use**: This editor must feel modern to the programmer despite being text-based (TUI), with a simple and modern UX influenced by VS Code for the best and most efficient programming experience in vintage hardware or terminal Linux environments.

## Project Environment & Compilation Workflow

- **Target System**: 32-bit Protected Mode MS-DOS (i386 386DX-33 minimum to Pentium-class machines) using the **DOS4GW** DPMI extender.
- **Compiler**: Watcom C 10.6 / OpenWatcom (for DOS), and GCC (for Linux).
- **Compilation Constraint**: The DOS binary is compiled inside a virtual machine (e.g., DOSBox-X) and cannot be compiled directly on the modern host operating system. The Linux binary can be compiled directly on the host using GCC.
- **Build Commands**:
  - **DOS**: Managed via the root `makefile`. Compilation is triggered in the VM by running:
    ```cmd
    wmake build
    ```
    This compiles all dependency (`deps/`), HAL/platform (`hal/`, `platform/dos/`), and core engine (`core/`) modules into `.OBJ` files inside `bin/dos/`, and links them to produce `dcode.exe`.
  - **Linux**: Managed via `makefile.linux`. Compilation is run using:
    ```bash
    make -f makefile.linux
    ```
    This compiles and links the Linux-specific source files to produce `dcode` inside `bin/linux/`.
 
## Coding Style Rules

The project strictly follows the coding standards defined in the sibling rules file `.agents/rules/c-code-stlye.md` (note the spelling typo in the filename):
- **Standard**: C89 (strict 90's compatibility).
- **Naming Scheme**: Function names must follow a module abbreviation prefix structure: `moduleabreviation_functionName` (e.g., `vid_putPixel` or `ed_typeChar`).
- **Defensive Coding**: Early error validation to avoid heavily nested `if` blocks (e.g., `if (!videoMemory) return 0;`).

## Advanced Architecture & Subsystems

### 1. Rendering & Buffering Pipeline
DCode IDE uses a multi-layered, flicker-free rendering pipeline:
- **File Buffer / Doubly Linked List**: Text is managed in memory as a doubly linked list of lines.
- **Editor Buffer (`editormemptr`)**: A local buffer containing the formatted/rendered editor text and UI components.
- **Video Buffer (`textmemptr` at `0xB8000` on DOS)**: Direct memory access write for VGA Text Mode 3 (or ANSI escape rendering on Linux).

### 2. Memory Arena Management
To prevent memory fragmentation in vintage DOS environments, a custom arena memory allocation system is implemented under `deps/mem/`:
- Each open file gets its own isolated memory "arena" (`FileArena`) to store its line structures and data.
- The editor allocates block structures from specific arenas (`MEM_ARENA_TEXT`, `MEM_ARENA_METADATA`, etc.) rather than calling standard `malloc`/`free` directly during active editing.

### 3. Keyboard Input & ISR Handling
- The editor employs a platform-specific keyboard driver defined under `platform/dos/input/` and `platform/linux/input/` through `hal/hal_inp.h`. On DOS, it uses a custom ISR (Interrupt Service Routine) to detect complex key combinations and modifier edge states (e.g., holding `Alt` while pressing `Shift`).
- Standard characters and arrow navigation keys are captured by querying `kbhit()` and `getch()` to leverage standard BIOS translation for different codepages (e.g., CP437).
- Buffer syncing is crucial: to prevent hotkeys (like `Ctrl+O`) from leaking as ghost inputs into dialog text prompts, the input buffer must be synchronized by waiting for key release (`inp_waitForRelease()`) and clearing the buffer (`inp_clearKeyboardBuffer()`) before opening prompts.

### 4. Direct VGA/VESA Video Modes
- Direct register manipulation is used to modify the CRT controller and sequencer registers for changing display resolutions under DOS (`platform/dos/video/`).
- Supports standard `80x25` as well as high-density modes (`80x43`, `80x50`, `132x50`, `132x60`, etc.), cycled using the `F11` hotkey (or specified via settings).

## Sub-dialogs & Core IDE Features

- **Quick Open Dialog (`Ctrl+O`)**: Reactive filesystem viewer that dynamically updates the list of files in the current folder as the user types an absolute path.
- **File Switcher (`Alt+Shift`)**: Visual overlay triggered by holding `Alt` and tapping `Shift` to cycle through currently open file arenas.
- **Search Tool (`Ctrl+F`)**: Dialogue that counts matching occurrences of a string in the current file; navigates matches forward with `Enter` and backward with `Shift+Enter`.
- **Selection Tool (`Shift + Navigation Keys`)**: Anchors a selection at the initial cursor position, allowing text block selection for editing operations.
- **Shell Spawn (`F9`)**: Suspends the IDE, restores standard 80x25 display settings, and spawns a command interpreter shell (using `COMSPEC` on DOS or `SHELL` on Linux) to let the user compile or test their code. Typing `exit` returns the user cleanly to the IDE, restoring their video mode, cursor, and editor state.
- **Memory Visualizer (`F12`)**: Displays active allocation maps and sizes of memory arenas (implemented in `core/vismem/vismem.c` and triggered in `app/main.c`).

## Folder & Component Structure

### Root
- **`makefile`**: Build instructions for Watcom C (MS-DOS).
- **`makefile.linux`**: Build instructions for GCC (Linux).
- **`readme.md`**: High-level details of key features, building, and running.
- **`.agents/rules/`**: Context guidelines for AI coding assistants.

### `app/`
- **`main.c` / `main.h`**: Application entry point, main event loop, global keybinding declarations, and keyboard ISR initialization/teardown.

### `hal/` (Hardware Abstraction Layer)
Defines unified cross-platform interfaces for hardware control:
- **`hal_fs.h`**: Filesystem and directory listing abstraction.
- **`hal_inp.h`**: Keyboard input and ISR driver abstraction.
- **`hal_vid.h`**: Text-mode/video mode abstraction.

### `platform/` (Platform-Specific Implementations)
- **`dos/`**: MS-DOS specific implementations for fs, input ISR, and VGA text mode video.
- **`linux/`**: Linux specific implementations for terminal keyboard input, ANSI escape video rendering, and filesystem operations.

### `core/` (Core Engine Modules)
- **`config/`**: Handling editor configuration options (smart closing, indentation, colors).
- **`draw/`**: Low-level screen buffering, borders, box drawing characters, window layouts, and editor token syntax highlighting.
- **`editor/`**: Central text manipulation (backspace, type character, newline), cursor navigation, search logic, selection logic, and shell spawning.
- **`files/`**: File system loading, saving, creating, closing, and tracking active `FileArena` lists.
- **`test/`**: Testing playground / debugger overlay.
- **`vismem/`**: Graphical visualization utility for memory arenas.
- **`std.h`**: Aggregate header of standard libraries.

### `deps/` (Generic/Reusable Dependencies)
- **`data/`**: Doubly linked list implementation for text lines and generic collections.
- **`env/`**: Deserialization of config files (`.cfg` / `.env`). Loads editor config.
- **`ext/`**: Extends standard C library functions (e.g., custom `vsnprintf` implementation).
- **`input/`**: Platform-independent input helpers and structures.
- **`log/`**: Global logger utility writing events to `LOGS.TXT`.
- **`mem/`**: Arena allocator base code for memory pooling.
- **`sort/`**: Generic sorting macros/functions.
- **`str/`**: Custom string utilities (trimming, slicing, reversing).

### `bin/` (Build Outputs)
- **`dos/`**: Intermediate object files (`.OBJ`) and the compiled `dcode.exe` DOS binary.
- **`linux/`**: Compiled `dcode` Linux binary.

### `books/`
- Reference manuals, cheatsheets, and architectural documentation for the editor's development.

### `pground/`
- A playground directory containing standalone code (`pground.c`) for testing.