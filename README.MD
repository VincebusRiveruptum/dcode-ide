# DCode IDE

### Modern IDE for DOS machines

DCODE IDE is a text editor and development environment designed for 32-bit DOS (386+ machines). It brings modern productivity features and shortcuts to the vintage DOS environment, bridging the gap between retro hardware and modern coding expectations.

## Key Features

- **Modern Editor Logic**: Support for standard shortcuts like `Home`/`End`, `PgUp`/`PgDn`, `Ctrl+N` (New), and `Ctrl+S` (Save).
- **Flexible Display Modes**: Easily switch between standard **25 lines**, and higher density **43** or **50 lines** text modes using `F1`, `F2`, and `F3`.
- **Real-time Syntax Highlighting**: Integrated C syntax highlighting with configurable color schemes for better readability.
- **Advanced Rendering Pipeline**: Uses a multi-layered buffering system (File Buffer → Editor Buffer → Video Buffer) for flicker-free, responsive performance.
- **Custom Input Driver**: Built on a custom ISR (Interrupt Service Routine) keyboard driver for precise control, combined with DOS codepage translation.
- **Intelligent Editing**: Features like auto-indenting, auto-braces, and configurable tab behavior.
- **Arena Memory Management**: Efficient memory usage through a custom arena allocation system, providing each open file with its own isolated memory "arena".

## Technical Stack

- **Standards**: C89 (Targeting the 90's style programming era).
- **Compiler**: Watcom C 10.6 / OpenWatcom.
- **Architecture**: 32-bit Protected Mode using the **DOS4GW** extender.
- **Hardware**: Aimed at i386 (386DX-33 Minimum) to Pentium-class machines.
- **Video**: VGA Text Mode 3 (Direct memory access to `0xB8000`).

## Project Structure

- **`APP/`**: Application entry point and main loop (`MAIN.C`).
- **`ENG/`**: The core engine, including the editor logic (`EDITOR.C`), rendering (`DRAW.C`), configuration (`CONFIG.C`), and video management (`VIDEO.C`).
- **`DEPS/`**: Hardware-level dependencies and utility libraries (Input, VGA, Memory, Logging).
- **`BIN/`**: Output directory for compiled object files and the final executable.
- **`BOOKS/`**: Project documentation, development diaries, and technical patterns.

## Configuration

The IDE can be configured via `BIN/DEFAULT.CFG`. Available settings include:
- `DEBUG`: Toggle debug logging.
- `AUTOIDENTING`: Enable/Disable automatic indentation.
- `AUTOCLOSING` / `AUTOBRACES`: Smart character closing.
- `VIDEO_MODE`: Default startup video mode.
- `STATUSBAR_COLOR_BG` / `TEXT`: UI customization.
- `clang_colors`: Custom syntax highlighting colors.

## Building from Source

To compile DCode IDE, you need a Watcom C environment (10.6 or OpenWatcom).

1. Open your DOS environment (or DOSBox-X).
2. Navigate to the project root.
3. Run the following command:
   ```cmd
   wmake build
   ```
This will clean previous builds, compile all modules, and generate `DCODE.EXE` in the root directory.

## Development Status

DCode IDE is an ongoing project created for educational and entertainment purposes. It follows a strict 90's C89 coding style and modular architecture.

**Author**: Vincebus Riveruptum (2026)




