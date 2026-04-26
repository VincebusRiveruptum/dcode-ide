
#include "DRAW.H"
// Private functions

// SENTENCE - WORD 
// tHIS WILL DETECT THE COMMENT ENDS IN THE SINGLE LINE

// Three posible scenarios
unsigned char _checkWordType(char *reservedWord){
    if(reservedWord[0] == '/' && reservedWord[1] == '/' ) return DW_RESWORD_SINGLE_LINE_COMMENT;       // INCLUDE
    if(reservedWord[0] == '"' && reservedWord[strlen(reservedWord) - 1] == '"' ) return DW_RESWORD_STRING;       // INCLUDE
    if(reservedWord[0] == '#' ) return DW_RESWORD_INCLUDE;       // INCLUDE
    if(reservedWord[0] == '\0') return DW_RESWORD_NONE;

    return DW_RESWORD_NONE;
}
bool _isWordEnd(char *csWordEnd){

    if (
        *csWordEnd != ' ' &&
        *csWordEnd != '\t' &&
        *csWordEnd != '\0' &&
        *csWordEnd != '\n' &&
        *csWordEnd != '\r' &&
        
        // DEPENDS
        *csWordEnd != '(' &&
        *csWordEnd != ')' &&
        *csWordEnd != '[' &&
        *csWordEnd != ']' &&
        *csWordEnd != '{' &&
        *csWordEnd != '}' &&
        //*csWordEnd != '\'' &&
        //*csWordEnd != ',' &&
        //*csWordEnd != '.' &&
        *csWordEnd != ';' &&
        *csWordEnd != ':' //&&
    )    return true;

    return false;
}

unsigned char _keywordMap(char *word, char *previousWord){
    int ivalue;
    float fvalue;
    short hvalue;
    long lvalue;
    char *endptr = word;
    
    if(!word) return 0;

    while(*endptr != '\0'){
        endptr++;
    }

    if(strcmp(previousWord, "#include") == 0){
        return COLOR_LIGHT_GREEN;
    }

    // Then we check if it's a keyword
    
    // vars
    if (strcmp(word, "int") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "long") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "short") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "double") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "FILE") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "size_t") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "float") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "char") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "void") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "bool") == 0) return COLOR_LIGHT_RED;
if (strcmp(word, "typedef") == 0) return COLOR_LIGHT_RED;
if (strcmp(word, "unsigned") == 0) return COLOR_LIGHT_RED;

    /* Control flow */
    if (strcmp(word, "if") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "else") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "while") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "do") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "for") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "return") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "break") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "switch") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "case") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "default") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "goto") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "continue") == 0) return COLOR_LIGHT_RED;
    
        // Preprocessor directives
    if (strcmp(word, "#include") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#define") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#ifdef") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#ifndef") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#if") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#endif") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#else") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#elif") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#undef") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#error") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#line") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#pragma") == 0) return COLOR_LIGHT_GREEN;
    if (strcmp(word, "#warning") == 0) return COLOR_LIGHT_YELLOW;
    if (strcmp(word, "#include_next") == 0) return COLOR_LIGHT_GREEN;
    
    // Expressions
    if (strcmp(word, "==") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "!=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, ">=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "<=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, ">") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "<") == 0) return COLOR_LIGHT_RED;
    
    // Operators
    if (strcmp(word, "+") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "-") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "*") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "/") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "%") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, ">>") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "<<") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "|") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "&") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "^") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "!") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "~") == 0) return COLOR_LIGHT_RED;
    
    // Logical operators
    if (strcmp(word, "&&") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "||") == 0) return COLOR_LIGHT_RED;
    
    // Assignment operators
    if (strcmp(word, "=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "+=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "-=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "*=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "/=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "%=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "&=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "|=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "^=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, "<<=") == 0) return COLOR_LIGHT_RED;
    if (strcmp(word, ">>") == 0) return COLOR_LIGHT_RED;
    
    // Boolean values
    if (strcmp(word, "true") == 0) return COLOR_LIGHT_BLUE;
    if (strcmp(word, "false") == 0) return COLOR_LIGHT_BLUE;
    
    // Null
    if (strcmp(word, "NULL") == 0) return COLOR_LIGHT_BLUE;
    if (strcmp(word, "0") == 0) return COLOR_LIGHT_BLUE;
    
    // Comments
    if (strcmp(word, "//") == 0) return COLOR_DARK_GRAY;
    if (strcmp(word, "/*") == 0) return COLOR_DARK_GRAY;
    if (strcmp(word, "*/") == 0) return COLOR_DARK_GRAY;
    
    if (strcmp(word, "renamon") == 0) return COLOR_LIGHT_YELLOW;



    // We first check if it's a value
    ivalue = (int)strtod(word, &endptr);
    
    /* 
    if(endptr != word && errno != ERANGE) {
        return COLOR_LIGHT_BLUE;
    }
    */
    return 0;
}

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
                    return 0xC9; //'É';
                case DW_SIDE_TOP_RIGHT:
                    return 0xBB;//'»';
                case DW_SIDE_BOTTOM_LEFT:
                    return 0xC8;//'È';
                case DW_SIDE_BOTTOM_RIGHT:
                    return 0xBC;//'¼';
                case DW_SIDE_TOP:
                    return 0xCD;//'Í';
                case DW_SIDE_BOTTOM:
                    return 0xCD; //'Í';
                case DW_SIDE_LEFT:
                    return 0xBA; //'º';
                case DW_SIDE_RIGHT:
                    return 0xBA; //'º';
                case DW_SIDE_ALL:
                    return 0xB0; //'°';
                default:
                    return 0xB0; //'°';
            }
        default:
            return 0xB0; //'°';
    }
}

void dw_cls(
    unsigned short *buffer){
    unsigned short blank;
    int i;

    blank = 0x20 | ((COLOR_BLACK << 4 | COLOR_LIGHT_GRAY) << 8);
    for(i = 0; i < VIDEO_COLS * VIDEO_ROWS; i++)
        buffer[i] = blank;

    ed_putCursor(0, 0);
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

    // f_dumpBufferTofile(&tempBuffer, VIDEO_BUFFER_SIZE, "temp1.txt");
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

void dw_charXY_color(unsigned short *buffer, char c, unsigned char x, unsigned char y, unsigned short color){
    unsigned short screenPos;
    unsigned short bgColor = (buffer[(y * VIDEO_COLS) + x] >> 12) & 0x0F;

    screenPos = (y * VIDEO_COLS) + x;
    buffer[screenPos] = c | ((bgColor << 4) | (color << 8));
}

void dw_charXY(unsigned short *buffer, char c, unsigned char x, unsigned char y){
    unsigned short screenPos;
    
    screenPos = (y * VIDEO_COLS) + x;
    buffer[screenPos] = c | (buffer[screenPos] & 0xFF00);

}

void dw_char(unsigned short *buffer, char c){
    unsigned short screenPos;
    
    screenPos = (currentCursorY * VIDEO_COLS) + currentCursorX;
    buffer[screenPos] = c | (buffer[screenPos] & 0xFF00);

}

char dw_read(unsigned short *buffer, int x, int y){
    char c;
    
    if(x < 0 && x >= VIDEO_COLS && y < 0 && y >= VIDEO_ROWS) return 0;

    c = (buffer[(y * VIDEO_COLS) + x] & 0xFF);
    
    return c;
}

char dw_readForegroundColor(unsigned short *buffer, int x, int y){
    char c;
    
    if(x < 0 && x >= VIDEO_COLS && y < 0 && y >= VIDEO_ROWS) return 0;
    
    c = ((buffer[(y * VIDEO_COLS) + x]) >> 8) & 0x0F;
    
    return c;
}

char dw_readBackgroundColor(unsigned short *buffer, int x, int y){
    char c;
    
    if(x < 0 && x >= VIDEO_COLS && y < 0 && y >= VIDEO_ROWS) return 0;
    
    c = ((buffer[(y * VIDEO_COLS) + x]) >> 12) & 0x0F;
    
    return c;
}

void dw_writeColor(unsigned short *buffer, int x, int y, unsigned short foregroundColor, unsigned short backgroundColor){
    unsigned short screenPos;
    unsigned short bgColor = (buffer[(y * VIDEO_COLS) + x] >> 12) & 0x0F;
    unsigned short fgColor = (buffer[(y * VIDEO_COLS) + x] >> 8) & 0x0F;

    if(foregroundColor <= 0x0F) fgColor = foregroundColor;
    if(backgroundColor <= 0x0F) bgColor = backgroundColor;

    screenPos = (y * VIDEO_COLS) + x;
    buffer[screenPos] = (buffer[screenPos] & 0x00FF) | (bgColor << 12) | (fgColor << 8);
}

/* This also will take care of reserved word coloring */
void dw_writeBufferEditorFormatted(unsigned short *destBuffer, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, File *file){

    int x, y;
    Node *currentNode = NULL;
    Line *line;
    int linePos;
    int screenX;
    int screenPos;
    char c;
    bool isSentence=false;
    
    int t;
    int j;
    int spaceBetween = 0;    
    int lineCount = 0;
    char lineCounterBufferTemp[8] = {0};
    
    // special word detection stuff
    char *csWordStart;
    char *csWordEnd;        // current character special word
    char *csStringEnd;        // current character special word
    char detectedWord[32] = {0};
    char previousWord[32] = {0};
    unsigned char specialWordColor = 0;   

    bool isMultilineComment = false;
    bool isSingleLineComment = false;   
    bool isString = false;
    /* Boundary check */
    if(x1 > x2 || y1 > y2) return;


    /**/
    if(!file->lines || file->lines->length == 0) {
        /* Clear area if no lines */
        for(y = y1; y <= y2; y++){
            for(x = x1; x <= x2; x++){
                destBuffer[(y * VIDEO_COLS) + x] = ' ' | ((backgroundColor << 4 | foregroundColor) << 8);
            }
        }
        return;
    }

    /*

    OK, AI had to help me with this part, but it's still WIP
    Pre-scan multiline comments and find current node
    
    */
    currentNode = file->lines->firstNode;
    lineCount = 0;
    while(lineCount < file->scrollY && currentNode) {
        line = (Line *)currentNode->data;
        for(j = 0; j < (int)line->length; j++) {
            if(!isMultilineComment && line->buffer[j] == '/' && line->buffer[j+1] == '*') {
                isMultilineComment = true;
                j++;
            } else if(isMultilineComment && line->buffer[j] == '*' && line->buffer[j+1] == '/') {
                isMultilineComment = false;
                j++;
            }
        }
        currentNode = currentNode->next;
        lineCount++;
    }

    /* End pre-scan */
    
    for(y = y1; y <= y2; y++){
        if(currentNode){
            line = (Line *)currentNode->data;
            linePos = 0;
            screenX = x1;
            spaceBetween = 0;
            csWordStart = 0;
            csWordEnd = 0;
            csStringEnd=NULL;
            isSingleLineComment = false;
            isString=false;
            
            while(screenX <= x2){
                //
                // ============ LINE COUNTER COLUMN ===============================================
                //
                
                screenPos = (y * VIDEO_COLS) + screenX;
                
                /* If is line counter column */
                if (screenX == x1){
                    /* TERRIBLE CODE I KNOW*/
                    for(j=0; j < LINE_COUNTER_WIDTH; j++){
                        destBuffer[screenPos + j] = ' ' | ((COLOR_BLACK << 4 | COLOR_WHITE) << 8);
                    }

                    sprintf(lineCounterBufferTemp, "%d", lineCount + 1);
                    
                    if (lineCount >= 9) spaceBetween = 1;
                    if (lineCount >= 99) spaceBetween = 2;
                    if (lineCount >= 999) spaceBetween = 3;
                    if (lineCount >= 9999) spaceBetween = 4;
                    if (lineCount >= 99999) spaceBetween = 5;

                    for(j=0; j < (int)strlen(lineCounterBufferTemp); j++){
                        destBuffer[screenPos + j + (LINE_COUNTER_WIDTH - spaceBetween - 1)] = lineCounterBufferTemp[j] | ((COLOR_BLACK << 4 | COLOR_WHITE) << 8);
                    }
                    
                    screenX += LINE_COUNTER_WIDTH;
                }else{
                //
                // ============ LINE CONTENT ========================================================
                //
                    /* After line counter column, we draw the rest of the line content */
                    if(linePos + file->scrollX < line->length){
                        c = line->buffer[linePos + file->scrollX];

                        // =========== SPECIAL KEYWORD COLORING=======================================
                        if(&line->buffer[linePos + file->scrollX] > csWordEnd){
                            memset(previousWord,'\0', 32);
                            memcpy(previousWord, detectedWord, strlen(detectedWord));
                            memset(detectedWord, '\0', 32);

                            csWordStart = &line->buffer[linePos + file->scrollX];
                            csWordEnd = csWordStart;

                            if(isMultilineComment == false){
                                if(*csWordStart == '/' && *(csWordStart + 1) == '*') isMultilineComment = true;
                            }

                            if(line->buffer[linePos + file->scrollX ] == '*' && line->buffer[linePos + file->scrollX + 1] == '/'){
                                isMultilineComment = false;
                            }
                            
                            // There is no comment in the current line
                            if(isMultilineComment == false /*&& isSingleLineComment == false*/){

                                if(!csStringEnd){
                                    isString = (*csWordEnd == '"') ? true : false;
                                }

                                if(isString == true){
                                    // We get where the string is supposed to end, this will run  just once.
                                    if(!csStringEnd){
                                        csStringEnd = csWordEnd;
                                        do{
                                            ++csStringEnd;       
                                        }while(*(csStringEnd) != '"' && *(csStringEnd) != '\0');
                                    }
                                    // If the character is under the addres where the string ends, then we treat the character
                                    // as string

                                    if(csWordEnd < csStringEnd){
                                        specialWordColor = COLOR_LIGHT_YELLOW;
                                    }else{
                                        csStringEnd = NULL;
                                        isString=false;
                                    }

                                }else{
                                    while( _isWordEnd(csWordEnd) == true){
                                        csWordEnd++;
                                    }
                                    memcpy(detectedWord, csWordStart, csWordEnd - csWordStart);
                                    
                                    detectedWord[csWordEnd - csWordStart] = '\0';
                                    
                                    if(_checkWordType(detectedWord) == DW_RESWORD_SINGLE_LINE_COMMENT) isSingleLineComment = true;
                                    
                                    specialWordColor = _keywordMap(detectedWord, previousWord);
                                }
                            }
                            
                            if(isMultilineComment || isSingleLineComment){
                                specialWordColor = COLOR_DARK_GRAY;
                            }
                        }

                        // We loop internally until we reach the end of the word

                        // If the word is actually  mapped and has its own color, we use that
                        
                        // If not, we just print the character with the default color
                                                
                        
                        if(c == '\t'){
                            for(t=0; t < 4 && screenX <= x2; t++){
                                destBuffer[(y * VIDEO_COLS) + screenX] = ' ' | ((backgroundColor << 4 | foregroundColor) << 8);
                                screenX++;
                            }
                            linePos++;
                            /* Subtract one because screenX is incremented in the loop logic below or will be incremented in next iteration */
                            /* Actually we just continue and screenX is already at the next position */
                            continue; 
                        } else if (c == '\r' || c == '\n') {
                            /* Skip these as we are already on a new line */
                            linePos++;
                            continue;
                        } else {

                            destBuffer[screenPos] = c | ((backgroundColor << 4 | (specialWordColor ? specialWordColor : foregroundColor)) << 8);
                            screenX++;
                            linePos++;
                        }
                    } else {
                        destBuffer[screenPos] = ' ' | ((backgroundColor << 4 | foregroundColor) << 8);
                        screenX++;
                    }
                }
                
            }
            currentNode = currentNode->next;
            lineCount++;
        } else {
            /* Fill remaining lines with spaces */
            for(x = x1; x <= x2; x++){
                destBuffer[(y * VIDEO_COLS) + x] = ' ' | ((backgroundColor << 4 | foregroundColor) << 8);
            }
        }
    }
}






