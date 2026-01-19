/*
    TUI Engine by Vincebus Riveruptum

    (C) 2026 Vincebus Riveruptum

    This library is for educational or entertainment purposes only.
    It is not intended for use in production environments.
*/

#ifndef TUI_H
#define TUI_H

#include <STDIO.H>
#include <STDLIB.H>
#include <CONIO.H>
#include <STRING.H>
#include <DOS.H>    
#include <MEM.H>
#include <i86.h>

// THIS REQUIRE THE INPUT DEPS
#include "..\INPUT\INPUT.H"
#include "..\SORT\SORT.H"
#include "..\DATA\DATA.H"

#define T_COLOR_BLACK 0x00
#define T_COLOR_BLUE 0x01
#define T_COLOR_GREEN 0x02
#define T_COLOR_CYAN 0x03
#define T_COLOR_RED 0x04
#define T_COLOR_MAGENTA 0x05
#define T_COLOR_BROWN 0x06
#define T_COLOR_LIGHT_GRAY 0x07
#define T_COLOR_DARK_GRAY 0x08
#define T_COLOR_LIGHT_BLUE 0x09
#define T_COLOR_LIGHT_GREEN 0x0A
#define T_COLOR_LIGHT_CYAN 0x0B
#define T_COLOR_LIGHT_RED 0x0C
#define T_COLOR_LIGHT_MAGENTA 0x0D
#define T_COLOR_LIGHT_YELLOW 0x0E
#define T_COLOR_WHITE 0x0F

/* TUI Characters (DOS Code Page 437) =====================*/
/* Format: UTF8 (Looks like) -> DOS (Copy this) -> (Code/Hex) - Description
 *
 * SINGLE LINE BORDERS:
 *   ┌ -> Ú -> (218/0xDA) - Top-left corner
 *   ┐ -> ¿ -> (191/0xBF) - Top-right corner
 *   └ -> À -> (192/0xC0) - Bottom-left corner
 *   ┘ -> Ù -> (217/0xD9) - Bottom-right corner
 *   ─ -> Ä -> (196/0xC4) - Horizontal line
 *   │ -> ³ -> (179/0xB3) - Vertical line
 *   ├ -> Ã -> (195/0xC3) - Left T-junction
 *   ┤ -> ´ -> (180/0xB4) - Right T-junction
 *   ┬ -> Â -> (194/0xC2) - Top T-junction
 *   ┴ -> Á -> (193/0xC1) - Bottom T-junction
 *   ┼ -> Å -> (197/0xC5) - Cross/intersection
 *
 * DOUBLE LINE BORDERS:
 *   ╔ -> É -> (201/0xC9) - Top-left corner
 *   ╗ -> » -> (187/0xBB) - Top-right corner
 *   ╚ -> È -> (200/0xC8) - Bottom-left corner
 *   ╝ -> ¼ -> (188/0xBC) - Bottom-right corner
 *   ═ -> Í -> (205/0xCD) - Horizontal line
 *   ║ -> º -> (186/0xBA) - Vertical line
 *   ╠ -> Ì -> (204/0xCC) - Left T-junction
 *   ╣ -> ¹ -> (185/0xB9) - Right T-junction
 *   ╦ -> Ë -> (203/0xCB) - Top T-junction
 *   ╩ -> Ê -> (202/0xCA) - Bottom T-junction
 *   ╬ -> Î -> (206/0xCE) - Cross/intersection
 *
 * MIXED SINGLE/DOUBLE BORDERS:
 *   ╒ -> Õ -> (213/0xD5) - Top-left (double horiz, single vert)
 *   ╓ -> Ö -> (214/0xD6) - Top-left (single horiz, double vert)
 *   ╕ -> ¸ -> (184/0xB8) - Top-right (double horiz, single vert)
 *   ╖ -> · -> (183/0xB7) - Top-right (single horiz, double vert)
 *   ╘ -> Ô -> (212/0xD4) - Bottom-left (double horiz, single vert)
 *   ╙ -> Ó -> (211/0xD3) - Bottom-left (single horiz, double vert)
 *   ╛ -> ¾ -> (190/0xBE) - Bottom-right (double horiz, single vert)
 *   ╜ -> ½ -> (189/0xBD) - Bottom-right (single horiz, double vert)
 *   ╞ -> Æ -> (198/0xC6) - Left T (double horiz, single vert)
 *   ╟ -> Ç -> (199/0xC7) - Left T (single horiz, double vert)
 *   ╡ -> µ -> (181/0xB5) - Right T (double horiz, single vert)
 *   ╢ -> ¶ -> (182/0xB6) - Right T (single horiz, double vert)
 *   ╤ -> Ñ -> (209/0xD1) - Top T (double horiz, single vert)
 *   ╥ -> Ò -> (210/0xD2) - Top T (single horiz, double vert)
 *   ╧ -> Ï -> (207/0xCF) - Bottom T (double horiz, single vert)
 *   ╨ -> Ð -> (208/0xD0) - Bottom T (single horiz, double vert)
 *   ╪ -> × -> (215/0xD7) - Cross (double horiz, single vert)
 *   ╫ -> Ø -> (216/0xD8) - Cross (single horiz, double vert)
 *
 * BLOCK ELEMENTS (Gradient/Shading):
 *   █ -> Û -> (219/0xDB) - Full block (100%)
 *   ▓ -> ² -> (178/0xB2) - Dark shade (75%)
 *   ▒ -> ± -> (177/0xB1) - Medium shade (50%)
 *   ░ -> ° -> (176/0xB0) - Light shade (25%)
 *   ▄ -> Ü -> (220/0xDC) - Lower half block
 *   ▀ -> ß -> (223/0xDF) - Upper half block
 *   ▌ -> Ý -> (221/0xDD) - Left half block
 *   ▐ -> Þ -> (222/0xDE) - Right half block
 *
 * SPECIAL CHARACTERS:
 *   • -> ù -> (249/0xF9) - Bullet point
 *   ■ -> þ -> (254/0xFE) - Small filled square
 *   ° -> ø -> (248/0xF8) - Degree symbol
 *   · -> ú -> (250/0xFA) - Middle dot
 *   √ -> û -> (251/0xFB) - Square root
 *   ≈ -> ÷ -> (247/0xF7) - Approximately equal
 *   ± -> ± -> (241/0xF1) - Plus-minus
 *   ÷ -> ö -> (246/0xF6) - Division sign
 */


 #define TUI_MAX_NAME_SIZE 16
 #define TUI_MAX_TEXT_SIZE 256
 #define TUI_MAX_TOOLTIP_SIZE 512
 #define TUI_MAX_STATUSBAR_TEXT_SIZE 80

/* Types ===================================================*/

typedef enum ButtonType {
    TUI_BUTTON_TYPE_DEFAULT,
    TUI_BUTTON_TYPE_PRIMARY,
    TUI_BUTTON_TYPE_DANGER,
    TUI_BUTTON_TYPE_WARNING,
    TUI_BUTTON_TYPE_SUCCESS
} ButtonType;

typedef enum InputType {
    TUI_INPUT_TYPE_TEXT,
    TUI_INPUT_TYPE_NUMBER,
    TUI_INPUT_TYPE_PASSWORD
} InputType;

typedef enum PositionType {
    TUI_POSITION_TYPE_ABSOLUTE,
    TUI_POSITION_TYPE_RELATIVE
} PositionType;

typedef enum JustifyType {
    TUI_JUSTIFY_EVENLY,
    TUI_JUSTIFY_BETWEEN,
    TUI_JUSTIFY_STRETCH
} JustifyType;

typedef enum AlignType {
    TUI_ALIGN_START,
    TUI_ALIGN_CENTER,
    TUI_ALIGN_END
} AlignType;

typedef enum DirectionType {
    TUI_DIRECTION_ROW,
    TUI_DIRECTION_COLUMN
} DirectionType;

typedef enum ElementType {
    TUI_ELEMENT_TYPE_LABEL,
    TUI_ELEMENT_TYPE_INPUT,
    TUI_ELEMENT_TYPE_BUTTON,
    TUI_ELEMENT_TYPE_STATUSBAR,
    TUI_ELEMENT_TYPE_MENU,
    TUI_ELEMENT_TYPE_DIALOG,
    TUI_ELEMENT_TYPE_FORM,
    TUI_ELEMENT_TYPE_FORM_GROUP,
    TUI_ELEMENT_TYPE_CONTAINER
} ElementType;

typedef struct Position {
    unsigned char type;
    unsigned short x;
    unsigned short y;
    short zIndex;
} Position;

/*
A form-group could be used for
- A group with a label and an input
- A group with a label and a button
- A group with a label and a checkbox
- A group with a label and a radio button
*/

typedef struct BaseElement {
    ElementType type;
    unsigned short id;
    struct Position position;
    char name[TUI_MAX_NAME_SIZE];
    bool enabled;
    bool visible;
} BaseElement;


typedef struct FormGroup {
    struct BaseElement base;
    JustifyType justifyContent;
    DirectionType direction;
} FormGroup;

typedef struct Form {
    struct Position position;
    struct FormGroup *group;
    bool enabled;
    bool visible;

    void (*submit)(void);
} Form;

typedef struct Label {
    struct BaseElement base;
    char text[TUI_MAX_TEXT_SIZE];
} Label;

typedef struct Input {
    struct BaseElement base;
    InputType iptType;
    char text[TUI_MAX_TEXT_SIZE];
    char tooltip[TUI_MAX_TOOLTIP_SIZE];

    // Value storage
    char strValue[TUI_MAX_TEXT_SIZE];
    int intValue;
    float floatValue;
    unsigned short length;
    
    void (*change)(void);
    
} Input;

typedef struct Button {
    struct BaseElement base;
    ButtonType btnType;
    char keys[4];    // Key combinations
    char text[TUI_MAX_TEXT_SIZE];
    char tooltip[TUI_MAX_TOOLTIP_SIZE];

    void (*click)(void);
} Button;

typedef struct Statusbar {
    struct BaseElement base;
    char text[TUI_MAX_STATUSBAR_TEXT_SIZE];    
} Statusbar;

typedef struct MenuItem {
    struct BaseElement base;
    char *text;
} MenuItem;

typedef struct Menu {
    struct BaseElement base;
    unsigned char alignment;
    struct List *items;
} Menu;

typedef struct Dialog {
    struct BaseElement base;
    char *title;
    bool closeButton;
    bool helpButton;

    struct Form *form;
    struct Position position;
    
    void (*helpFunction)(void);
} Dialog;

typedef struct Container {
    struct BaseElement base;
    struct List *elements;
} Container;

/* Global =================================================*/

extern unsigned short *textmemptr;


extern int attrib;
extern unsigned char TUI_COLS;
extern unsigned char TUI_ROWS;

// Element Root
extern struct Container *root;

/* ASM Prototypes */

// Rembember, there is a 'clear' bit :
// AX = 0x0003 = clear screen
// AX = 0x0083 = don't clear screen

#pragma aux tg_set350Lines_asm = \
    "mov ax, 0x1201" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \

#pragma aux tg_set400Lines_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \

#pragma aux tg_set43Lines_asm = \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    
#pragma aux tg_set25Lines_asm = \
    "mov ax, 0x1114" \
    "mov bl, 0x00" \
    "int 0x10" \


#endif

/* Prototypes ==============================================*/

void tg_init_video();
void tg_close_video();
void tg_cls();
void tg_moveCursor(unsigned char x, unsigned char y);
void tg_drawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char background, unsigned char foreground, unsigned char character,  bool blinking);


void tg_set43Lines();
void tg_set25Lines();

void tg_set43Lines_asm();
void tg_set25Lines_asm();

// render prototypes

void tg_renderElements();
void tg_renderContainer(Container *container);
void tg_renderLabeL(Label *label);
void tg_renderButton(Button *button);
void tg_renderInput(Input *input);
void tg_renderMenu(Menu *menu);
void tg_renderDialog(Dialog *dialog);
void tg_renderForm(Form *form);
void tg_renderFormGroup(FormGroup *formGroup);