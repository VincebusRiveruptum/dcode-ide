# DCode IDE

### Modern IDE for DOS and Linux machines

DCODE IDE is a text editor and development environment designed for 32-bit DOS (386+ machines) and Linux terminal environments. It brings modern productivity features and shortcuts to vintage text-mode and terminal interfaces, bridging the gap between retro hardware and modern coding expectations.

## Key Features

- **Modern Editor Logic**: Support for standard shortcuts like `Home`/`End`, `PgUp`/`PgDn`, `Ctrl+N` (New), and `Ctrl+S` (Save).
- **Split Windows (`Ctrl + \`)**: Split the editor vertically. Spawning a split duplicates the active tab, sharing the exact same in-memory file space so that edits are instantly synchronized across panes.
- **Window Switching (`Ctrl + W`)**: Easily switch cursor focus between active split panes. Closing a split (closing all its tabs) dynamically reclaims screen space, expanding adjacent splits back to full width.
- **Non-blocking Switcher (`Alt+Shift`)**: Hold `Alt` and press `Shift` (or `Alt+s` / `Alt+Tab` fallback) to open a list of open tabs in the focused split. Cycles asynchronously using the main loop.
- **Flexible Display Modes**: Cycle display modes with `F11` (supports standard 80x25, as well as high-density 80x43, 80x50, 132x50, and 132x60 modes on DOS).
- **Real-time Syntax Highlighting**: Integrated C syntax highlighting with configurable color schemes for better readability.
- **Advanced Rendering Pipeline**: Uses a multi-layered buffering system (File Buffer → Editor Buffer → Video Buffer) for flicker-free, responsive performance.
- **Custom Input Driver**: Built on a custom ISR (Interrupt Service Routine) keyboard driver for precise control on DOS, combined with standard terminal escape handling on Linux.
- **Intelligent Editing**: Features like auto-indenting, auto-braces, and configurable tab behavior.
- **Dynamic Arena Memory Management**: Efficient memory usage through a custom arena allocation system. Each open file gets its own dynamically allocated, isolated memory arena.

## Technical Stack

- **Standards**: C89 (Targeting the 90's style programming era).
- **Compiler**: Watcom C 10.6 / OpenWatcom (for DOS), and GCC (for Linux).
- **Architecture**: 32-bit Protected Mode using the **DOS4GW** extender.
- **Hardware**: Aimed at i386 (386DX-33 Minimum) to Pentium-class machines.
- **Video**: VGA Text Mode 3 (Direct memory access to `0xB8000` on DOS) and ANSI terminal rendering (on Linux).

## Project Structure

- **`app/`**: Application entry point and main event loop (`main.c` / `main.h`).
- **`hal/`**: Hardware Abstraction Layer mapping fs, input, and video interfaces.
- **`platform/`**: Platform-specific implementations (`dos/` and `linux/`).
- **`core/`**: Core engine modules including editor buffers (`editor/`), formatters/drawers (`draw/`), workspace settings (`config/`), and file tab structures (`files/`).
- **`deps/`**: Personal generic/portable framework dependencies (List collections, Memory Arenas, Logger).
- **`bin/`**: Output directory for compiled object files and binaries (`bin/dos/dcode.exe`, `bin/linux/dcode`).
- **`books/`**: Project documentation and architecture specs.

## Configuration

The IDE can be configured via `bin/default.cfg`. Available settings include:
- `DEBUG`: Toggle debug logging.
- `AUTOIDENTING`: Enable/Disable automatic indentation.
- `AUTOCLOSING` / `AUTOBRACES`: Smart character closing.
- `VIDEO_MODE`: Default startup video mode.
- `STATUSBAR_COLOR_BG` / `TEXT`: UI customization.
- `clang_colors`: Custom syntax highlighting colors.

## Building from Source

### DOS (Watcom / OpenWatcom)
1. Open your DOS environment (or DOSBox-X).
2. Navigate to the project root.
3. Run `wmake build`. This compiles all object files into `bin/dos/` and links `dcode.exe`.

### Linux (GCC)
1. Open your Linux terminal.
2. Navigate to the project root.
3. Run `make -f makefile.linux`. This compiles and links the Linux binary `dcode` inside `bin/linux/`.

## Development Status

DCode IDE is an ongoing project created for educational and entertainment purposes. It follows a strict 90's C89 coding style and modular architecture.

**Author**: Vincebus Riveruptum (2026)




