
#include "DRAW.H"

#define BUFFER_SIZE 2048

char _getBorderCharacter(BorderType borderType, RectangleSides side){
    switch(borderType){
        case DRAW_BORDER_SIMPLE:
            switch(side){
                case DRAW_SIDE_TOP_LEFT:
                    return 218;
                case DRAW_SIDE_TOP_RIGHT:
                    return 191;
                case DRAW_SIDE_BOTTOM_LEFT:
                    return 192;
                case DRAW_SIDE_BOTTOM_RIGHT:
                    return 217;
                case DRAW_SIDE_TOP:
                    return 196;
                case DRAW_SIDE_BOTTOM:
                    return 196;
                case DRAW_SIDE_LEFT:
                    return 179;
                case DRAW_SIDE_RIGHT:
                    return 179;
                case DRAW_SIDE_ALL:
                    return '°';
                default:
                    return '°';
            }
        case DRAW_BORDER_DOUBLE:
            switch(side){
                case DRAW_SIDE_TOP_LEFT:
                    return 'É';
                case DRAW_SIDE_TOP_RIGHT:
                    return '»';
                case DRAW_SIDE_BOTTOM_LEFT:
                    return 'È';
                case DRAW_SIDE_BOTTOM_RIGHT:
                    return '¼';
                case DRAW_SIDE_TOP:
                    return 'Í';
                case DRAW_SIDE_BOTTOM:
                    return 'Í';
                case DRAW_SIDE_LEFT:
                    return 'º';
                case DRAW_SIDE_RIGHT:
                    return 'º';
                case DRAW_SIDE_ALL:
                    return '°';
                default:
                    return '°';
            }
        default:
            return '°';
    }
}

void dw_cls(){
    unsigned short blank;
    int i;

    blank = CHAR_SPACE | (attrib << 8);
    for(i = 0; i < VIDEO_COLS * VIDEO_ROWS; i++)
        textmemptr[i] = blank;

    ed_moveCursor(0, 0);
}

void dw_fill(unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character){
    unsigned short screenCharacter;
    int i;

    screenCharacter = character | ((backgroundColor << 4 | foregroundColor) << 8);
    
    for(i = 0; i < VIDEO_COLS * VIDEO_ROWS; i++)
        textmemptr[i] = screenCharacter;
}


void dw_rectangle(
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

    i = (y1 * VIDEO_COLS) + x1;

    //If i is the first on the index draw top left corner
    textmemptr[i] = _getBorderCharacter(borderType, DRAW_SIDE_TOP_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
    
    i++;

    for(i; i < (y2 * VIDEO_COLS) + x2; i++){
        leftLimit = i % VIDEO_COLS >= x1;
        rightLimit = i % VIDEO_COLS <= x2;
        
        if(rightLimit && leftLimit){
            // If i is the top line draw top line
            if(i < (y1 * VIDEO_COLS) + x1 + VIDEO_COLS){
                if(i % VIDEO_COLS == x2){
                    screenCharacter = _getBorderCharacter(borderType, DRAW_SIDE_TOP_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }else{
                    screenCharacter = _getBorderCharacter(borderType, DRAW_SIDE_TOP) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }
            }

            // Bottom side
            else if( i > ((y2 * VIDEO_COLS) + x2 )- VIDEO_COLS){
                // If i is the bottom line draw bottom line
                if(i % VIDEO_COLS == x1){
                    screenCharacter = _getBorderCharacter(borderType, DRAW_SIDE_BOTTOM_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }else{
                    screenCharacter = _getBorderCharacter(borderType, DRAW_SIDE_BOTTOM) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    textmemptr[i] = screenCharacter;
                }

            }
            
            // If i is the left line draw left line
            else if(i % VIDEO_COLS <= x1){
                screenCharacter = _getBorderCharacter(borderType, DRAW_SIDE_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                textmemptr[i] = screenCharacter;
            }
            // If i is the right line draw right line
            else if(i % VIDEO_COLS >= x2){
                screenCharacter = _getBorderCharacter(borderType, DRAW_SIDE_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                textmemptr[i] = screenCharacter;
            }else{
                screenCharacter = fillCharacter | ((backgroundFillColor << 4 | foregroundFillColor) << 8);
                textmemptr[i] = screenCharacter;
            }                          
        }
    }
    
    textmemptr[i] = _getBorderCharacter(borderType, DRAW_SIDE_BOTTOM_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
}

// This functions copies content from a buffer to textmemptr then displays it on screen
void dw_writeBuffer(const char *format, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char foregroundColor, unsigned char backgroundColor, ...){

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
            screenPos = (y * VIDEO_COLS) + x;
            
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

void dw_char(char c){
    unsigned short screenPos;
    
    screenPos = (currentCursorY * VIDEO_COLS) + currentCursorX;
    textmemptr[screenPos] = c | (textmemptr[screenPos] & 0xFF00);
    currentCursorX++;
}
    
