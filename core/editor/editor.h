
#ifndef ENG_EDITOR_H
#define ENG_EDITOR_H

#include "../std.h"

// THIS REQUIRE THE INPUT DEPS
#include "../../app/main.h"

#define ED_STATUSBAR_WIDTH 132
#define ED_WORD_JUMP_PREV 0
#define ED_WORD_JUMP_NEXT 1
#define ED_LINE_JUMP_UP 0
#define ED_LINE_JUMP_DOWN 1

// Activity values

#define ED_ACTIVITY_TYPE 2
#define ED_ACTIVITY_DEL 3
#define ED_ACTIVITY_SUPR 4
#define ED_ACTIVITY_NEWLINE 5
#define ED_ACTIVITY_SWAP_DOWN 6


/*
 * IBM PC / DOS Code Page 437
 * Line Drawing & Special Characters
 *
 * NOTE:
 * - Decimal / Hex values are CP437
 * - The "ASCII-looking" character shown (Ú, Ä, etc.)
 *   is what appears in Latin-1 / Windows when viewed incorrectly
 * - In DOS text mode, these render as box-drawing glyphs
 */

/* =========================================================
 * SINGLE LINE BORDERS
 * ========================================================= 
 *   ┌ -> Ú -> (218 / 0xDA) - Top-left corner
 *   ┐ -> ¿ -> (191 / 0xBF) - Top-right corner
 *   └ -> À -> (192 / 0xC0) - Bottom-left corner
 *   ┘ -> Ù -> (217 / 0xD9) - Bottom-right corner
 *   ─ -> Ä -> (196 / 0xC4) - Horizontal line
 *   │ -> ³ -> (179 / 0xB3) - Vertical line
 *   ├ -> Ã -> (195 / 0xC3) - Left T-junction
 *   ┤ -> ´ -> (180 / 0xB4) - Right T-junction
 *   ┬ -> Â -> (194 / 0xC2) - Top T-junction
 *   ┴ -> Á -> (193 / 0xC1) - Bottom T-junction
 *   ┼ -> Å -> (197 / 0xC5) - Cross / intersection
*/

/* =========================================================
 * DOUBLE LINE BORDERS
 * ========================================================= *
 *   ╔ -> É -> (201 / 0xC9) - Top-left corner
 *   ╗ -> » -> (187 / 0xBB) - Top-right corner
 *   ╚ -> È -> (200 / 0xC8) - Bottom-left corner
 *   ╝ -> ¼ -> (188 / 0xBC) - Bottom-right corner
 *   ═ -> Í -> (205 / 0xCD) - Horizontal line
 *   ║ -> º -> (186 / 0xBA) - Vertical line
 *   ╠ -> Ì -> (204 / 0xCC) - Left T-junction
 *   ╣ -> ¹ -> (185 / 0xB9) - Right T-junction
 *   ╦ -> Ë -> (203 / 0xCB) - Top T-junction
 *   ╩ -> Ê -> (202 / 0xCA) - Bottom T-junction
 *   ╬ -> Î -> (206 / 0xCE) - Cross / intersection
*/

/* =========================================================
 * MIXED SINGLE / DOUBLE BORDERS
 * ========================================================= 
 *   ╒ -> Õ -> (213 / 0xD5) - Top-left (double horiz, single vert)
 *   ╓ -> Ö -> (214 / 0xD6) - Top-left (single horiz, double vert)
 *   ╕ -> ¸ -> (184 / 0xB8) - Top-right (double horiz, single vert)
 *   ╖ -> · -> (183 / 0xB7) - Top-right (single horiz, double vert)
 *   ╘ -> Ô -> (212 / 0xD4) - Bottom-left (double horiz, single vert)
 *   ╙ -> Ó -> (211 / 0xD3) - Bottom-left (single horiz, double vert)
 *   ╛ -> ¾ -> (190 / 0xBE) - Bottom-right (double horiz, single vert)
 *   ╜ -> ½ -> (189 / 0xBD) - Bottom-right (single horiz, double vert)
 *   ╞ -> Æ -> (198 / 0xC6) - Left T (double horiz, single vert)
 *   ╟ -> Ç -> (199 / 0xC7) - Left T (single horiz, double vert)
 *   ╡ -> µ -> (181 / 0xB5) - Right T (double horiz, single vert)
 *   ╢ -> ¶ -> (182 / 0xB6) - Right T (single horiz, double vert)
 *   ╤ -> Ñ -> (209 / 0xD1) - Top T (double horiz, single vert)
 *   ╥ -> Ò -> (210 / 0xD2) - Top T (single horiz, double vert)
 *   ╧ -> Ï -> (207 / 0xCF) - Bottom T (double horiz, single vert)
 *   ╨ -> Ð -> (208 / 0xD0) - Bottom T (single horiz, double vert)
 *   ╪ -> × -> (215 / 0xD7) - Cross (double horiz, single vert)
 *   ╫ -> Ø -> (216 / 0xD8) - Cross (single horiz, double vert)
*/

/* =========================================================
 * BLOCK ELEMENTS (Shading / Blocks)
 * ========================================================= 
 *   █ -> Û -> (219 / 0xDB) - Full block (100%)
 *   ▓ -> ² -> (178 / 0xB2) - Dark shade (75%)
 *   ▒ -> ± -> (177 / 0xB1) - Medium shade (50%)
 *   ░ -> ° -> (176 / 0xB0) - Light shade (25%)
 *   ▄ -> Ü -> (220 / 0xDC) - Lower half block
 *   ▀ -> ß -> (223 / 0xDF) - Upper half block
 *   ▌ -> Ý -> (221 / 0xDD) - Left half block
 *   ▐ -> Þ -> (222 / 0xDE) - Right half block
*/

/* =========================================================
 * SPECIAL CHARACTERS (CP437)
 * ========================================================= 
 *   • -> ù -> (249 / 0xF9) - Bullet
 *   ■ -> þ -> (254 / 0xFE) - Solid square
 *   ° -> ø -> (248 / 0xF8) - Degree symbol
 *   · -> ú -> (250 / 0xFA) - Middle dot
 *   √ -> û -> (251 / 0xFB) - Square root
 *   ≈ -> ÷ -> (247 / 0xF7) - Approximately equal
 *   ± -> ± -> (241 / 0xF1) - Plus-minus
 *   ÷ -> ö -> (246 / 0xF6) - Division
 */

/* Types ===================================================*/

extern struct Container *root;
extern bool ed_renderEvent;
extern bool on_selection_tool;
extern bool on_search_tool;

extern time_t ed_globalAuxTimer;
extern char statusBarMessage[ED_STATUSBAR_WIDTH];
/* Prototypes ==============================================*/

void ed_initConfig(int argc, char *argv[]);
void ed_handleArguments(int argc, char *argv[]);

void ed_putCursor(unsigned char x, unsigned char y);
void ed_moveCursor(short x, short y);
void ed_typeChar(char c);
void ed_backspace();
void ed_supr();
void ed_tab();
void ed_newLine();
void ed_resetActity();
void ed_prepareSearchTool();

void ed_resetCursor();

char *ed_getDefaultExtension();
char *ed_scanf(unsigned char x, unsigned char y, unsigned char maxChars);
char *ed_async_scanf(unsigned char x, unsigned char y, unsigned char maxChars, char *buffer, size_t bufflen, int *stepIndex);

void ed_putCursorEnd();
void ed_putCursorStart();

void ed_putCursorFistLine();
void ed_putCursorLastLine();
void ed_statusBarMessage(const char *format,  ...);
bool ed_checkStatusBarMessage();
void ed_statusBar();

void ed_wordJump(short wordJump);
void ed_swapLine(short lineJump);

void ed_prepareSelectionTool();
void ed_clearSelection();
void ed_handleSelection();

void ed_drawSearchTool();
int ed_wordCountInStr(char *str);
void ed_shellSpawn();
void ed_renderElements();
void _updateCursor();
#endif

