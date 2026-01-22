#ifndef TUI_EL_H
#define TUI_EL_H

#include "STD.H"
#include "TUI.H"

/* Consts  ========================================================*/

 #define TUI_MAX_NAME_SIZE 16
 #define TUI_MAX_TEXT_SIZE 256
 #define TUI_MAX_TOOLTIP_SIZE 512
 #define TUI_MAX_STATUSBAR_TEXT_SIZE 80

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

/* Render types ========================================================*/

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

/* Render prototypes ========================================================*/

void tg_renderElements();
void tg_renderContainer(Container *container);
void tg_renderLabeL(Label *label);
void tg_renderButton(Button *button);
void tg_renderInput(Input *input);
void tg_renderMenu(Menu *menu);
void tg_renderDialog(Dialog *dialog);
void tg_renderForm(Form *form);
void tg_renderFormGroup(FormGroup *formGroup);

#endif