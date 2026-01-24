#include "EL.H"

void el_renderFiles_test(){
    short i = 0;
    Node *fileNode = NULL;
    File *file = NULL;

    /* For now we will use this way of list travel
        altrough we know its O(N) we will use it for now
    */
    logger("\n[el_renderFiles_test]: Rendering %d files", MAX_ARENAS);

    for(i; i < MAX_ARENAS; i++){
        fileNode = getNodeByIndex(&fileList, i);
        file = (File*)fileNode->data;

        if(!file) continue;

        dw_writeBuffer("%s", 0, 2, VIDEO_COLS, VIDEO_ROWS - 2, 0, COLOR_LIGHT_GRAY, COLOR_BLUE, file->buffer);
    }
}

void el_renderContainer(Container *container){
    return;
}
void el_renderLabeL(Label *label){
    // Put the cursor on labels coordinates
    // Print the text with the color attributes

    return;
}
void el_renderButton(Button *button){
    // Put the cursor on the button coordinates
    // Draw a rectangle with the color attributes at the position
    // Print the text with the color attributes
    return;
}
void el_renderInput(Input *input){
    return;
}
void el_renderStatusbar(Statusbar *menu){
    return;
}
void el_renderMenu(Menu *menu){
    return;
}
void el_renderDialog(Dialog *dialog){
    return;
}
void el_renderForm(Form *form){
    return;
}
void el_renderFormGroup(FormGroup *formGroup){
    return;
}
void el_renderElements(){
    int i=0;
    BaseElement *element;
    Node *node;
    if(!root || !root->elements) return;

    for(i=0; i < root->elements->length; i++){
        node = getNodeByIndex(root->elements, i);
        element = (BaseElement*)node->data;

        if(!element) continue;

        switch(element->type){
            case EL_TYPE_CONTAINER:
                el_renderContainer((Container*)element);
                break;
            case EL_TYPE_LABEL:
                el_renderLabel((Label*)element);
                break;
            case EL_TYPE_BUTTON:
                el_renderButton((Button*)element);
                break;
            case EL_TYPE_INPUT:
                el_renderInput((Input*)element);
                break;
            case EL_TYPE_STATUSBAR:
                el_renderStatusbar((Statusbar*)element);
                break;
            case EL_TYPE_MENU:
                el_renderMenu((Menu*)element);
                break;
            case EL_TYPE_DIALOG:
                el_renderDialog((Dialog*)element);
                break;
            case EL_TYPE_FORM:
                el_renderForm((Form*)element);
                break;
            case EL_TYPE_FORM_GROUP:
                el_renderFormGroup((FormGroup*)element);
                break;
            default:
                break;
        }
    }
}   

void el_rootZOrder(){
    /*
        This method orders elements in the root list by z-order
    */
    S_SORT_LIST(root->elements, BaseElement, position.zIndex, SORT_TYPE_INT, true);
}
