#ifndef ENG_EL_H
#define ENG_EL_H

#include "STD.H"
#include "..\APP\MAIN.H"

/* Consts  ========================================================*/

 #define EL_MAX_NAME_SIZE 16
 #define EL_MAX_TEXT_SIZE 256
 #define EL_MAX_TOOLTIP_SIZE 512
 #define EL_MAX_STATUSBAR_TEXT_SIZE 132

typedef enum ButtonType {
    EL_BUTTON_TYPE_DEFAULT,
    EL_BUTTON_TYPE_PRIMARY,
    EL_BUTTON_TYPE_DANGER,
    EL_BUTTON_TYPE_WARNING,
    EL_BUTTON_TYPE_SUCCESS
} ButtonType;

typedef enum InputType {
    EL_INPUT_TYPE_TEXT,
    EL_INPUT_TYPE_NUMBER,
    EL_INPUT_TYPE_PASSWORD
} InputType;

typedef enum PositionType {
    EL_POSITION_TYPE_ABSOLUTE,
    EL_POSITION_TYPE_RELATIVE
} PositionType;

typedef enum JustifyType {
    EL_JUSTIFY_EVENLY,
    EL_JUSTIFY_BETWEEN,
    EL_JUSTIFY_STRETCH
} JustifyType;

typedef enum AlignType {
    EL_ALIGN_START,
    EL_ALIGN_CENTER,
    EL_ALIGN_END
} AlignType;

typedef enum DirectionType {
    EL_DIRECTION_ROW,
    EL_DIRECTION_COLUMN
} DirectionType;

typedef enum ElementType {
    EL_TYPE_LABEL,
    EL_TYPE_INPUT,
    EL_TYPE_BUTTON,
    EL_TYPE_STATUSBAR,
    EL_TYPE_MENU,
    EL_TYPE_DIALOG,
    EL_TYPE_FORM,
    EL_TYPE_FORM_GROUP,
    EL_TYPE_CONTAINER
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
    char name[EL_MAX_NAME_SIZE];
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
    char text[EL_MAX_TEXT_SIZE];
} Label;

typedef struct Input {
    struct BaseElement base;
    InputType iptType;
    char text[EL_MAX_TEXT_SIZE];
    char tooltip[EL_MAX_TOOLTIP_SIZE];

    // Value storage
    char strValue[EL_MAX_TEXT_SIZE];
    int intValue;
    float floatValue;
    unsigned short length;
    
    void (*change)(void);
    
} Input;

typedef struct Button {
    struct BaseElement base;
    ButtonType btnType;
    char keys[4];    // Key combinations
    char text[EL_MAX_TEXT_SIZE];
    char tooltip[EL_MAX_TOOLTIP_SIZE];

    void (*click)(void);
} Button;

typedef struct Statusbar {
    struct BaseElement base;
    char text[EL_MAX_STATUSBAR_TEXT_SIZE];    
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

void el_renderElements();
void el_renderContainer(Container *container);
void el_renderLabeL(Label *label);
void el_renderButton(Button *button);
void el_renderInput(Input *input);
void el_renderMenu(Menu *menu);
void el_renderDialog(Dialog *dialog);
void el_renderForm(Form *form);
void el_renderFormGroup(FormGroup *formGroup);

#endif