
#include "DRAW.H"

#define BUFFER_SIZE 2048

unsigned char currentCursorX = 0;
unsigned char currentCursorY = 0;

void tg_cls(){
    unsigned short blank;
    int i;

    blank = CHAR_SPACE | (attrib << 8);
    for(i = 0; i < TUI_COLS * TUI_ROWS; i++)
        textmemptr[i] = blank;

    tg_moveCursor(0, 0);
}

void tg_fill(unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character){
    unsigned short screenCharacter;
    int i;

    screenCharacter = character | ((backgroundColor << 4 | foregroundColor) << 8);
    
    for(i = 0; i < TUI_COLS * TUI_ROWS; i++)
        textmemptr[i] = screenCharacter;
}

char tg_getBorderCharacter(BorderType borderType, RectangleSides side){
    switch(borderType){
        case TUI_DRAW_BORDER_SIMPLE:
            switch(side){
                case TUI_DRAW_SIDE_TOP_LEFT:
                    return 218;
                case TUI_DRAW_SIDE_TOP_RIGHT:
                    return 191;
                case TUI_DRAW_SIDE_BOTTOM_LEFT:
                    return 192;
                case TUI_DRAW_SIDE_BOTTOM_RIGHT:
                    return 217;
                case TUI_DRAW_SIDE_TOP:
                    return 196;
                case TUI_DRAW_SIDE_BOTTOM:
                    return 196;
                case TUI_DRAW_SIDE_LEFT:
                    return 179;
                case TUI_DRAW_SIDE_RIGHT:
                    return 179;
                case TUI_DRAW_SIDE_ALL:
                    return '°';
                default:
                    return '°';
            }
        case TUI_DRAW_BORDER_DOUBLE:
            switch(side){
                case TUI_DRAW_SIDE_TOP_LEFT:
                    return 'É';
                case TUI_DRAW_SIDE_TOP_RIGHT:
                    return '»';
                case TUI_DRAW_SIDE_BOTTOM_LEFT:
                    return 'È';
                case TUI_DRAW_SIDE_BOTTOM_RIGHT:
                    return '¼';
                case TUI_DRAW_SIDE_TOP:
                    return 'Í';
                case TUI_DRAW_SIDE_BOTTOM:
                    return 'Í';
                case TUI_DRAW_SIDE_LEFT:
                    return 'º';
                case TUI_DRAW_SIDE_RIGHT:
                    return 'º';
                case TUI_DRAW_SIDE_ALL:
                    return '°';
                default:
                    return '°';
            }
        default:
            return '°';
    }
}

void tg_drawRectangle(
    unsigned short x1, 
    unsigned short y1, 
    unsigned short x2,
    unsigned short y2, 
    unsigned char backgroundBorderColor, 
    unsigned char foregroundBorderColor, 
    unsigned char fillCharacter,  
    unsigned char foregroundFillColor, 
    unsigned char backgroundFillColor, 
    bool blinking, 
    BorderType borderType){
    unsigned short screenCharacter;
    unsigned short i, leftLimit, rightLimit;

    // Boundary check
    if(x1 > x2 || y1 > y2) return;

    i = (y1 * TUI_COLS) + x1;

    //If i is the first on the index draw top left corner
    textmemptr[i] = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_TOP_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
    
    i++;

    for(i; i < (y2 * TUI_COLS) + x2; i++){
        leftLimit = i % TUI_COLS >= x1;
        rightLimit = i % TUI_COLS <= x2;
        
        if(rightLimit && leftLimit){
            // If i is the top line draw top line
            if(i < (y1 * TUI_COLS) + x1 + TUI_COLS){
                if(i % TUI_COLS == x2){
                    screenCharacter = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_TOP_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }else{
                    screenCharacter = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_TOP) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }
            }

            // Bottom side
            else if( i > ((y2 * TUI_COLS) + x2 )- TUI_COLS){
                // If i is the bottom line draw bottom line
                if(i % TUI_COLS == x1){
                    screenCharacter = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_BOTTOM_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }else{
                    screenCharacter = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_BOTTOM) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }

            }
            
            // If i is the left line draw left line
            else if(i % TUI_COLS <= x1){
                screenCharacter = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                textmemptr[i] = screenCharacter;
            }
            // If i is the right line draw right line
            else if(i % TUI_COLS >= x2){
                screenCharacter = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                textmemptr[i] = screenCharacter;
            }else{
                screenCharacter = fillCharacter | ((backgroundFillColor << 4 | foregroundFillColor) << 8);
                textmemptr[i] = screenCharacter;
            }                          
        }
    }
    
    textmemptr[i] = tg_getBorderCharacter(borderType, TUI_DRAW_SIDE_BOTTOM_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
}

// This functions copies content from a buffer to textmemptr then displays it on screen
void tg_writeBuffer(const char *format, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char foregroundColor, unsigned char backgroundColor, ...){

    va_list args;
    unsigned short screenCharacter;
    char buffer[BUFFER_SIZE];
    unsigned char x, y;
    unsigned short buffpos = 0;
    unsigned short screenPos;
    
    memset(buffer, 0, BUFFER_SIZE);

    va_start(args, backgroundColor);
    vsprintf(buffer, format, args); 
    va_end(args);

    // Boundary check
    if(x1 > x2 || y1 > y2) return;

    for(y = y1; y <= y2; y++){
        for(x = x1; x <= x2; x++){
            screenPos = (y * TUI_COLS) + x;
            
            if(buffpos < BUFFER_SIZE){
                textmemptr[screenPos] = buffer[buffpos] | ((backgroundColor << 4 | foregroundColor) << 8);
                buffpos++;
            } else {
                // Fill remaining area with spaces if buffer ends
                textmemptr[screenPos] = ' ' | ((backgroundColor << 4 | foregroundColor) << 8);
            }
        }
    }
}

void tg_putChar(char c){
    unsigned short screenPos;
    
    screenPos = (currentCursorY * TUI_COLS) + currentCursorX;
    textmemptr[screenPos] = c | (textmemptr[screenPos] & 0xFF00);
    currentCursorX++;
}
    
