
#include "DRAW.H"
    
char _getBorderCharacter(BorderType borderType, RectangleSides side){
    switch(borderType){
        case DRAW_BORDER_SIMPLE:
            switch(side){
                case DW_SIDE_TOP_LEFT:
                    return 218;
                case DW_SIDE_TOP_RIGHT:
                    return 191;
                case DW_SIDE_BOTTOM_LEFT:
                    return 192;
                case DW_SIDE_BOTTOM_RIGHT:
                    return 217;
                case DW_SIDE_TOP:
                    return 196;
                case DW_SIDE_BOTTOM:
                    return 196;
                case DW_SIDE_LEFT:
                    return 179;
                case DW_SIDE_RIGHT:
                    return 179;
                case DW_SIDE_ALL:
                    return '°';
                default:
                    return '°';
            }
        case DRAW_BORDER_DOUBLE:
            switch(side){
                case DW_SIDE_TOP_LEFT:
                    return 'É';
                case DW_SIDE_TOP_RIGHT:
                    return '»';
                case DW_SIDE_BOTTOM_LEFT:
                    return 'È';
                case DW_SIDE_BOTTOM_RIGHT:
                    return '¼';
                case DW_SIDE_TOP:
                    return 'Í';
                case DW_SIDE_BOTTOM:
                    return 'Í';
                case DW_SIDE_LEFT:
                    return 'º';
                case DW_SIDE_RIGHT:
                    return 'º';
                case DW_SIDE_ALL:
                    return '°';
                default:
                    return '°';
            }
        default:
            return '°';
    }
}

void dw_cls(
    unsigned short *buffer){
    unsigned short blank;
    int i;

    blank = CHAR_SPACE | (COLOR_BLACK << 4 | COLOR_LIGHT_GRAY) << 8;
    for(i = 0; i < VIDEO_COLS * VIDEO_ROWS; i++)
        buffer[i] = blank;

    ed_moveCursor(0, 0);
}

void dw_fill(
    unsigned short *buffer,
    unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character){
    unsigned short screenCharacter;
    int i;

    screenCharacter = character | ((backgroundColor << 4 | foregroundColor) << 8);
    
    for(i = 0; i < VIDEO_COLS * VIDEO_ROWS; i++)
        buffer[i] = screenCharacter;
}


void dw_rectangle(
    unsigned short *buffer,
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
    buffer[i] = _getBorderCharacter(borderType, DW_SIDE_TOP_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
    
    i++;

    for(i; i < (y2 * VIDEO_COLS) + x2; i++){
        leftLimit = i % VIDEO_COLS >= x1;
        rightLimit = i % VIDEO_COLS <= x2;
        
        if(rightLimit && leftLimit){
            // If i is the top line draw top line
            if(i < (y1 * VIDEO_COLS) + x1 + VIDEO_COLS){
                if(i % VIDEO_COLS == x2){
                    screenCharacter = _getBorderCharacter(borderType, DW_SIDE_TOP_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    buffer[i] = screenCharacter;
                }else{
                    screenCharacter = _getBorderCharacter(borderType, DW_SIDE_TOP) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    buffer[i] = screenCharacter;
                }
            }

            // Bottom side
            else if( i > ((y2 * VIDEO_COLS) + x2 )- VIDEO_COLS){
                // If i is the bottom line draw bottom line
                if(i % VIDEO_COLS == x1){
                    screenCharacter = _getBorderCharacter(borderType, DW_SIDE_BOTTOM_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    buffer[i] = screenCharacter;
                }else{
                    screenCharacter = _getBorderCharacter(borderType, DW_SIDE_BOTTOM) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    buffer[i] = screenCharacter;
                }

            }
            
            // If i is the left line draw left line
            else if(i % VIDEO_COLS <= x1){
                screenCharacter = _getBorderCharacter(borderType, DW_SIDE_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                buffer[i] = screenCharacter;
            }
            // If i is the right line draw right line
            else if(i % VIDEO_COLS >= x2){
                screenCharacter = _getBorderCharacter(borderType, DW_SIDE_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                buffer[i] = screenCharacter;
            }else{
                screenCharacter = fillCharacter | ((backgroundFillColor << 4 | foregroundFillColor) << 8);
                buffer[i] = screenCharacter;
            }                          
        }
    }
    
    buffer[i] = _getBorderCharacter(borderType, DW_SIDE_BOTTOM_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
}

void dw_writeBuffer(unsigned short *buffer, const char *format, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, ...){

    va_list args;
    unsigned short screenCharacter;
    unsigned char x, y;
    unsigned short buffpos = 0;
    unsigned short screenPos;
 
    memset(tempBuffer, 0, VIDEO_BUFFER_SIZE);

    va_start(args, backgroundColor);
    vsnprintf(tempBuffer, VIDEO_BUFFER_SIZE, format, args);
    va_end(args);

    // f_bufferDumpToFile(&tempBuffer, VIDEO_BUFFER_SIZE, "temp1.txt");
    // Boundary check
    if(x1 > x2 || y1 > y2) return;

    for(y = y1; y <= y2; y++){
        for(x = x1; x <= x2; x++){
            screenPos = (y * VIDEO_COLS) + x;
            
            if(buffpos < VIDEO_BUFFER_SIZE){
                buffer[screenPos] = tempBuffer[buffpos] | ((backgroundColor << 4 | foregroundColor) << 8);
                buffpos++;
            } else {
                // Fill remaining area with spaces if buffer ends
                buffer[screenPos] = ' ' | ((backgroundColor << 4 | foregroundColor) << 8);
            }
        }
    }
}

void dw_char(unsigned short *buffer, char c){
    unsigned short screenPos;
    
    screenPos = (currentCursorY * VIDEO_COLS) + currentCursorX;
    buffer[screenPos] = c | (buffer[screenPos] & 0xFF00);
    currentCursorX++;
}
    
