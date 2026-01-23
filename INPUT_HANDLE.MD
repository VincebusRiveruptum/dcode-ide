# DOS Input Handling: ASCII vs. Scan Codes

When building a TUI (Text User Interface) for DOS, handling keyboard input correctly is critical to avoid "ghost" characters and unexpected cursor movement.

## The Problem: Ghost Characters and Cursor Drift
In the previous implementation, pressing keys like **F1, F2, or Arrow Keys** would cause the cursor to move one space to the right and leave a "blank" character behind. This happened because of how the BIOS `getch()` function interacts with DOS.

## 1. ASCII vs. Scan Codes
Standard C library functions like `getch()` (from `conio.h`) return different values depending on the key pressed:

*   **Standard Keys (A-Z, 0-9, etc.)**: Return their standard **ASCII** value.
*   **Special Keys (F1-F12, Arrows, etc.)**: These are "extended keys." They don't have a single ASCII representation.

The code was previously using **Scan Codes** (which represent the physical position on the keyboard) to check values returned by `getch()`. For example, it was checking if `c == KEY_ESC` (where `KEY_ESC` is 1). However, the ASCII value for Escape is actually **27**.

## 2. The Two-Byte Extended Sequence
When you press an extended key, BIOS actually puts **two bytes** into the keyboard buffer:
1.  **The Identifier**: Always `0` (or `0xE0` on newer systems).
2.  **The Scan Code**: The actual code for the key (e.g., `72` for Up Arrow).

If your code calls `getch()` once, it only receives the `0`. If you then call `putch(0)`, the BIOS interprets 0 as a character (often a blank space) and moves the cursor.

## 3. The Solution
To fix this, we implemented a "look-ahead" check in the input loop:

```c
if(c == 0 || (unsigned char)c == 0xE0){
    /* 
     * This is an extended key! 
     * We must call getch() a second time to "eat" the 
     * scan code so it doesn't get treated as 
     * a character by the next loop.
     */
    getch(); 
} else {
    /* 
     * Standard ASCII character - now we check if it's 
     * a printable character or an action character 
     * (like Enter or Backspace) before printing it.
     */
    if(!(c == CHAR_ESCAPE || c == CHAR_BACKSPACE ...)){
        putch(c);
    }
}
```

### Key Takeaways
- **Always consume both bytes** of an extended key sequence.
- **Never pass 0 or 0xE0 to `putch()`** unless you explicitly want to print a special glyph from the CP437 character set.
- **Use ASCII constants** for comparing `getch()` results, not hardware scan codes.
