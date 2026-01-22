#include "EL.H"

void tg_renderContainer(Container *container){
    return;
}
void tg_renderLabeL(Label *label){
    // Put the cursor on labels coordinates
    // Print the text with the color attributes

    return;
}
void tg_renderButton(Button *button){
    // Put the cursor on the button coordinates
    // Draw a rectangle with the color attributes at the position
    // Print the text with the color attributes
    return;
}
void tg_renderInput(Input *input){
    return;
}
void tg_renderStatusbar(Statusbar *menu){
    return;
}
void tg_renderMenu(Menu *menu){
    return;
}
void tg_renderDialog(Dialog *dialog){
    return;
}
void tg_renderForm(Form *form){
    return;
}
void tg_renderFormGroup(FormGroup *formGroup){
    return;
}
void tg_renderElements(){
    int i=0;
    BaseElement *element;
    Node *node;
    if(!root || !root->elements) return;

    for(i=0; i < root->elements->length; i++){
        node = getNodeByIndex(root->elements, i);
        element = (BaseElement*)node->data;

        if(!element) continue;

        switch(element->type){
            case TUI_ELEMENT_TYPE_CONTAINER:
                tg_renderContainer((Container*)element);
                break;
            case TUI_ELEMENT_TYPE_LABEL:
                tg_renderLabel((Label*)element);
                break;
            case TUI_ELEMENT_TYPE_BUTTON:
                tg_renderButton((Button*)element);
                break;
            case TUI_ELEMENT_TYPE_INPUT:
                tg_renderInput((Input*)element);
                break;
            case TUI_ELEMENT_TYPE_STATUSBAR:
                tg_renderStatusbar((Statusbar*)element);
                break;
            case TUI_ELEMENT_TYPE_MENU:
                tg_renderMenu((Menu*)element);
                break;
            case TUI_ELEMENT_TYPE_DIALOG:
                tg_renderDialog((Dialog*)element);
                break;
            case TUI_ELEMENT_TYPE_FORM:
                tg_renderForm((Form*)element);
                break;
            case TUI_ELEMENT_TYPE_FORM_GROUP:
                tg_renderFormGroup((FormGroup*)element);
                break;
            default:
                break;
        }
    }
}   

void tg_rootZOrder(){
    /*
        This method orders elements in the root list by z-order
    */
    S_SORT_LIST(root->elements, BaseElement, position.zIndex, SORT_TYPE_INT, true);
}
