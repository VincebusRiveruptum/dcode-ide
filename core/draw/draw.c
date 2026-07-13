#include "draw.h"

bool dw_renderEvent = true;
RenderType dw_renderEventType=DW_RENDER_ALL;

// Private functions

// SENTENCE - WORD 
// tHIS WILL DETECT THE COMMENT ENDS IN THE SINGLE LINE

// Three posible scenarios
// TERRIBLE I KNOW BUT WORKS
unsigned char _isEscapeChar(char *c, bool *isIdentifier){
    bool isEscapeChar = false;

    // ESCAPE CHAR DETECTION
    if(*(c) == '\\' || 
        (*c) == 'a' ||
        (*c) == 'b' ||
        (*c) == 'e' ||
        (*c) == 'f' ||
        (*c) == 'n' ||
        (*c) == 'r' ||
        (*c) == 't' ||
        (*c) == 'v' ||
        (*c) == '\''||
        (*c) == '"' ||
        (*c) == '?' ||
        (*c) == '0'
    ){
        if((*c) == '\\'){
            switch(*(c + 1)){
                case 'a':
                case 'b':
                case 'e':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case 'v':
                case '\'':
                case '"':
                case '?':
                case '0':
                    isEscapeChar = true;
                    break;
            }
        }else{
            if(*(c - 1 ) == '\\') isEscapeChar = true;
        }

        return isEscapeChar ? COLOR_LIGHT_RED : 0;
    };

    // string parameter identifier detection
    if(*(c) == '%'){
        *isIdentifier = true;
        return COLOR_BROWN;
    }

    if(*isIdentifier == true){
        if(
            *(c - 1) != 'd' &&
            *(c - 1) != 'C' &&
            *(c - 1) != 'n' &&
            *(c - 1) != 'f' &&
            *(c - 1) != 'x' &&
            *(c - 1) != 'o' &&
            *(c - 1) != 's' &&
            *(c - 1) != 'l'
        ) {
            *isIdentifier = false;
            return COLOR_BROWN;
        }
        return COLOR_BROWN;
    }

    return 0;
}

bool _isExpression(char *csWordEnd){
    char c = *csWordEnd;

    switch(c) {
        case '+':
        case '-':
        case '%':
        case '~':
        case '^':
        case '=':
        case '!':
        case '<':
        case '>':
        case '&':
        case '|':
        case '?':
        case ':':
        case '.':
        case ',':
        case ';':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
            return true;
        default:
            return false;
    }
    return false;
}

bool _isComment(char *csWordEnd){
    char c = *csWordEnd;
    char n = *(csWordEnd + 1);

    switch(c) {
        /* Operators that need lookahead to avoid comment delimiters */
        case '*':
            /* Avoid coloring * in */
            if (n == '/') return false;
            return true;

        case '/':
            /* Avoid coloring / in // or slash-star comments */
            if (n == '/' || n == '*') return false;
            return true;
        default:
            return false;
    }
    return false;
}

bool _isWordEnd(char *csWordEnd){
    if (
        *(csWordEnd) == ' ' ||
        *(csWordEnd) == '\t' ||
        *(csWordEnd) == '\0' ||
        *(csWordEnd) == '\n' ||
        *(csWordEnd) == '\r' ||
        
        // DEPENDS
        _isExpression(csWordEnd)
    )    return true;

    return false;
}

// THIS DETECTS WORDS THAT ARE MORE THAN ONE CHARACTER
unsigned char _keywordMap(char *word){
    if(!word) return DW_RESWORD_NONE;

    // CONST
    if(isupper(*word) && isupper(*(word + 1))) return DW_RESWORD_CONSTANT;
    if(atoi(word)) return DW_RESWORD_INT;
    if(atof(word)) return DW_RESWORD_FLOAT;
    // Then we check if it's a keyword
    if(*word == '/' && *(word + 1) == '/') return DW_RESWORD_COMMENT;
    if(*word == '\'' && word[strlen(word) - 1] == '\'') return DW_RESWORD_CHAR;
    
    // DW_RESWORD_TYPES
    if (strcmp(word, "int") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "long") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "short") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "float") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "double") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "char") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "void") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "bool") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "unsigned") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "const") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "enum") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "struct") == 0) return DW_RESWORD_TYPES;
    if (strcmp(word, "union") == 0) return DW_RESWORD_TYPES;
    
    // Known STD types
    if (strcmp(word, "FILE") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "ptrdiff_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "wchar_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "size_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "clock_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "time_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "tm") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "fpos_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "div_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "ldiv_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "sig_atomic_t") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "jmp_buf") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "va_list") == 0) return DW_RESWORD_STD_FUNC;
    if (strcmp(word, "lconv") == 0) return DW_RESWORD_STD_FUNC;
    
    // known DOS.H types
    if (strcmp(word, "REGS") == 0) return DW_RESWORD_DOS_FUNC;
    if (strcmp(word, "WORDREGS") == 0) return DW_RESWORD_DOS_FUNC;
    if (strcmp(word, "BYTEREGS") == 0) return DW_RESWORD_DOS_FUNC;
    if (strcmp(word, "SREGS") == 0) return DW_RESWORD_DOS_FUNC;
    if (strcmp(word, "REGPACK") == 0) return DW_RESWORD_DOS_FUNC;
    if (strcmp(word, "asm") == 0) return DW_RESWORD_DOS_FUNC;
    
    /* Control flow */
    if (strcmp(word, "if") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "else") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "while") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "do") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "for") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "return") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "break") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "switch") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "case") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "default") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "goto") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "continue") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "typedef") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "extern") == 0) return DW_RESWORD_CONTROL_FLOW;
    if (strcmp(word, "volatile") == 0) return DW_RESWORD_CONTROL_FLOW;
    
    // Preprocessor directives
    if (strcmp(word, "#include") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#define") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#ifdef") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#ifndef") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#if") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#endif") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#else") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#elif") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#undef") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#error") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#line") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#pragma") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#include_next") == 0) return DW_RESWORD_PREPROCESSOR;
    if (strcmp(word, "#warning") == 0) return DW_RESWORD_PREPROCESSOR;
    
    if (*word == '0') return DW_RESWORD_INT;
    
    // Boolean values
    if (strcmp(word, "true") == 0) return DW_RESWORD_CONSTANT;
    if (strcmp(word, "false") == 0) return DW_RESWORD_CONSTANT;

    // Null
    if (strcmp(word, "NULL") == 0) return DW_RESWORD_CONSTANT;

    // Comments
    if (strcmp(word, "//") == 0) return DW_RESWORD_COMMENT;
    if (strcmp(word, "/*") == 0) return DW_RESWORD_COMMENT;
    if (strcmp(word, "*/") == 0) return DW_RESWORD_COMMENT;
    
    if (strcmp(word, "renamon") == 0) return DW_RESWORD_WIFE;

    return DW_RESWORD_NONE;
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
                    return 0xB0;
                default:
                    return 0xB0;
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

bool dw_isCharSelected(struct File *file, int lineIndex, int colIndex) {
    int startLine, endLine, startCol, endCol;

    if (!file || !file->selectedStartNode || !file->selectedEndNode) {
        return false;
    }

    if (file->selectedStartLine < file->selectedEndLine) {
        startLine = (int)file->selectedStartLine;
        startCol = (int)file->selectedStartX;
        endLine = (int)file->selectedEndLine;
        endCol = (int)file->selectedEndX;
    } else if (file->selectedStartLine > file->selectedEndLine) {
        startLine = (int)file->selectedEndLine;
        startCol = (int)file->selectedEndX;
        endLine = (int)file->selectedStartLine;
        endCol = (int)file->selectedStartX;
    } else {
        startLine = (int)file->selectedStartLine;
        endLine = (int)file->selectedEndLine;
        if (file->selectedStartX < file->selectedEndX) {
            startCol = (int)file->selectedStartX;
            endCol = (int)file->selectedEndX;
        } else {
            startCol = (int)file->selectedEndX;
            endCol = (int)file->selectedStartX;
        }
    }

    if (lineIndex > startLine && lineIndex < endLine) {
        return true;
    }
    if (lineIndex == startLine && lineIndex == endLine) {
        return (colIndex >= startCol && colIndex < endCol);
    }
    if (lineIndex == startLine) {
        return (colIndex >= startCol);
    }
    if (lineIndex == endLine) {
        return (colIndex < endCol);
    }

    return false;
}
bool dw_isCharFound(struct File *file, int lineIndex, int colIndex) {
    SearchMetadata *fileSearch = NULL;

    if (!file)
        return false;
    
    fileSearch = file->currentFileSearch;

    if(
        !fileSearch ||
        !fileSearch->currentWordNode ||
        !fileSearch->currentWordNode->data
    ) return false;
    
    if(
        (lineIndex == 
			(int)((WordMetadata *)fileSearch->currentWordNode->data)->cursorLine)
         &&(colIndex >= 
			(int)((WordMetadata *)fileSearch->currentWordNode->data)->cursorCol)
         && (colIndex < 
			(int)((WordMetadata *)fileSearch->currentWordNode->data)->cursorCol 
			+ (int)strlen(fileSearch->dialogInputBuffer))
    ) {
        logger(
			"LEN; %d", 
			(int)((WordMetadata *)fileSearch->currentWordNode->data)->cursorCol 
			+ (int)strlen(fileSearch->dialogInputBuffer)
		);
        return true;
    }

    return false;
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
    BorderType borderType,
    const char *title
    ){

    unsigned short screenCharacter = 0;
    unsigned short i = 0;
    unsigned short leftLimit = 0;
    unsigned short rightLimit = 0;
    unsigned short width = 0;
    short titleStartPos = 0;
    short titleCharIndex = 0;
    size_t titleLen = 0;

    (void)blinking;

    // Boundary check
    if(x1 > x2 || y1 > y2) return;
    
    width = x2 - x1;

    if(title){
        titleLen = strlen(title);
        
        // if title is centered we do the following:
        // we take the half of window width and the half of the title lenght
        // Then the start position of the tittle text is : windowwHalfWidth - titleHalfWidth
        // Same with the end position but suming instead,
        // Because the center is the origin now so we need to substact or add depending to the offset direction.
    
        titleStartPos = x1 + (short) (width / 2) - (short) (titleLen / 2);
    } 

    i = (y1 * VIDEO_COLS) + x1;

    //If i is the first on the index draw top left corner
    buffer[i] = _getBorderCharacter(borderType, DW_SIDE_TOP_LEFT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
    
    i++;

    for(; i < (y2 * VIDEO_COLS) + x2; i++){
        leftLimit = i % VIDEO_COLS >= x1;
        rightLimit = i % VIDEO_COLS <= x2;
        
        if(rightLimit && leftLimit){
            // If i is the top line draw top line
            if(i < (y1 * VIDEO_COLS) + x1 + VIDEO_COLS){
                if(i % VIDEO_COLS == x2){
                    screenCharacter = _getBorderCharacter(borderType, DW_SIDE_TOP_RIGHT) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                    buffer[i] = screenCharacter;
                }else{
                    // WE DRAW THE TITLE IF i is stepping on the boundaries of the title, of not we just draw the TOP of the rectangle
                    titleCharIndex = (short)(i % VIDEO_COLS) - titleStartPos;
                        
                    if(titleLen > 0 && (titleCharIndex >= 0 && titleCharIndex < (short)titleLen)){
                        buffer[i] = ((title[titleCharIndex] != '\0') ? title[titleCharIndex] : ' ') | ((0 | foregroundBorderColor) << 8);
                    }else{   
                        screenCharacter = _getBorderCharacter(borderType, DW_SIDE_TOP) | ((backgroundBorderColor << 4 | foregroundBorderColor) << 8);
                        buffer[i] = screenCharacter;
                    }
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
    unsigned short buffpos = 0;
    unsigned short screenPos = 0;
    unsigned char x = 0;
    unsigned char y = 0;
    va_list args;
 
    memset(tempBuffer, 0, HAL_VIDEO_BUFFER_SIZE);

    va_start(args, backgroundColor);
    vsnprintf(tempBuffer, HAL_VIDEO_BUFFER_SIZE, format, args);
    va_end(args);

    // f_dumpBufferTofile(&tempBuffer, HAL_VIDEO_BUFFER_SIZE, "temp1.txt");
    // Boundary check
    if(x1 > x2 || y1 > y2) return;

    for(y = y1; y <= y2; y++){
        for(x = x1; x <= x2; x++){
            screenPos = (y * VIDEO_COLS) + x;
            
            if(buffpos < HAL_VIDEO_BUFFER_SIZE){
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


void dw_c_formatter(unsigned short *destBuffer, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, File *file){
    int x = 0;
    int y = 0;
    int linePos = 0;
    int screenX = 0;
    int screenPos = 0;
    int t = 0;
    int j = 0;
    int spaceBetween = 0;    
    int lineCount = 0;
    unsigned char specialWordColor = 0;   
    unsigned char detectedWordType = DW_RESWORD_NONE;
    unsigned char prevDetectedWordType = DW_RESWORD_NONE;
    unsigned char stringEscapeCharColor = 0;
    unsigned short tabAttrib = 0;
    unsigned short charAttrib = 0;
    char lineCounterBufferTemp[16];
    char detectedWord[32];
    char previousWord[32];
    bool isMultilineComment = false;
    bool isSingleLineComment = false;   
    bool isString = false;
    bool isIdentifier = false;
    char *c = NULL;
    char *csWordStart = NULL;
    char *csWordEnd = NULL;
    char *csStringEnd = NULL;
    Node *currentNode = NULL;
    Line *line = NULL;

    memset(lineCounterBufferTemp, '\0', 16);
    memset(detectedWord, '\0', 32);
    memset(previousWord, '\0', 32);

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
            csWordStart = NULL;
            csWordEnd = NULL;
            csStringEnd=NULL;
            isSingleLineComment = false;
            isString=false;
            stringEscapeCharColor = 0;
            prevDetectedWordType = DW_RESWORD_NONE;
            detectedWordType = DW_RESWORD_NONE;
            isIdentifier=false;
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
                    if(linePos + (int)file->scrollX < (int)line->length){
                        c = &line->buffer[linePos + file->scrollX];

                        // =========== SPECIAL KEYWORD COLORING=======================================
                        if(c > csWordEnd){
                            memset(previousWord,'\0', 32);
                            memcpy(previousWord, detectedWord, strlen(detectedWord));
                            memset(detectedWord, '\0', 32);

                            csWordStart = c;
                            csWordEnd = csWordStart;
                            prevDetectedWordType = detectedWordType;
                            detectedWordType = DW_RESWORD_NONE;

                            
                            if(isMultilineComment == false){
                                if(*csWordStart == '/' && *(csWordStart + 1) == '*') isMultilineComment = true;
                            }

                            if(line->buffer[linePos + file->scrollX ] == '*' && line->buffer[linePos + file->scrollX + 1] == '/'){
                                isMultilineComment = false;
                            }
                            
                            // There is no comment in the current line
                            if(isMultilineComment == false /*&& isSingleLineComment == false*/){

                                /* STRING DETECTION METHOD*/
                                if(!csStringEnd){
                                    isString = (*csWordEnd == '"') && !(prevDetectedWordType == DW_RESWORD_PREPROCESSOR) ? true : false;
                                }

                                // EXCAPE CHAR DETECTION INSIDE STRING DOUBLE QUOTES 
                                stringEscapeCharColor = _isEscapeChar(csWordEnd, &isIdentifier);
                                
                                if(isString == true && stringEscapeCharColor){
                                    specialWordColor = stringEscapeCharColor;
                                }else if(isString == true){
                                    // We get where the string is supposed to end, this will run  just once.
                                    if(!csStringEnd){
                                        csStringEnd = csWordEnd;
                                        do{
                                            ++csStringEnd;       
                                        }while(*(csStringEnd) != '"' && *(csStringEnd) != '\0');
                                    }
                                    // If the character is under the addres where the string ends, then we treat the character
                                    // as string

                                    if(csWordEnd <= csStringEnd){
                                        specialWordColor = settings.clang_colors[DW_RESWORD_STRING];
                                    }else{
                                        csStringEnd = NULL;
                                        isString=false;
                                    }
                                // FULL #INLUCDE HANDLING (YELLOWING OF THE SECOND PART OF THE #INCLUDE SENTENCE)
                                }else if(prevDetectedWordType == DW_RESWORD_PREPROCESSOR){
                                    detectedWordType = (*csWordEnd == '>') ? 0 : DW_RESWORD_PREPROCESSOR;
                                    specialWordColor = settings.clang_colors[DW_RESWORD_STRING];

                                // MATH EXPRESSIONS, SYNTAX AND CONTROL CHARACTERS RED COLORING
                                }else if(
                                    (_isExpression(csWordEnd) ||
                                    _isComment(csWordEnd)) && 
                                    !(prevDetectedWordType == DW_RESWORD_PREPROCESSOR)){
                                        specialWordColor = settings.clang_colors[DW_RESWORD_EXPRESSION];
                                /* KEYWORD DETECTION METHOD (SIMPLE )*/
                                }else{
                                    // 1. Find the end of the current word
                                    while(*csWordEnd != '\0' && !_isWordEnd(csWordEnd)){
                                        csWordEnd++;
                                    }

                                    // 2. Extract the word safely
                                    t = (int)(csWordEnd - csWordStart);
                                    if (t > 31) t = 31;
                                    memcpy(detectedWord, csWordStart, t);
                                    detectedWord[t] = '\0';
                                                
                                    // 3. Identify the word type
                                    detectedWordType = _keywordMap(detectedWord);
                                    
                                    // 4. Use the settings color array
                                    specialWordColor = settings.clang_colors[detectedWordType];

                                    if(detectedWordType == DW_RESWORD_COMMENT){
                                        isSingleLineComment = true;
                                    }

                                    csWordEnd--;
                                }
                            }

                            if(isMultilineComment || isSingleLineComment){
                                specialWordColor = COLOR_DARK_GRAY;
                            }
                        }

                        // We loop internally until we reach the end of the word

                        // If the word is actually  mapped and has its own color, we use that
                        
                        // If not, we just print the character with the default color                    
                        
                        if(*c == '\t'){
                            /// UGLYT TEST
                            for(t=0; t < 4 && screenX <= x2; t++){
                                if (
                                    dw_isCharSelected(file, lineCount, linePos + file->scrollX) ||
                                    dw_isCharFound(file, lineCount, linePos + file->scrollX)
                                ){
                                    tabAttrib = ((COLOR_LIGHT_GRAY << 4) | (COLOR_BLACK << 8));
                                } else {
                                    tabAttrib = backgroundColor << 4 | ((t==3 && settings.TAB_INDICATOR == true) ? COLOR_DARK_GRAY : foregroundColor);
                                }
                                destBuffer[(y * VIDEO_COLS) + screenX] = ((t==3 && settings.TAB_INDICATOR == true) ? 179 : ' ') | (tabAttrib << 8);
                                screenX++;
                            }
                            linePos++;
                            /* Subtract one because screenX is incremented in the loop logic below or will be incremented in next iteration */
                            /* Actually we just continue and screenX is already at the next position */
                            continue; 
                        } else if (*c == '\r' || *c == '\n') {
                            /* Skip these as we are already on a new line */
                            linePos++;
                            continue;
                        } else {
                            if (
                                dw_isCharSelected(file, lineCount, linePos + file->scrollX) ||
                                dw_isCharFound(file, lineCount, linePos + file->scrollX)
                            ){
                                charAttrib = ((COLOR_LIGHT_GRAY << 4) | (COLOR_BLACK << 8));
                            } else {
                                charAttrib = backgroundColor << 4 | (specialWordColor ? specialWordColor : foregroundColor);
                            }
                            destBuffer[screenPos] = *c | (charAttrib << 8);
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


// SIMPLER
void dw_txt_formatter(unsigned short *destBuffer, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, File *file){
    int x = 0;
    int y = 0;
    int linePos = 0;
    int screenX = 0;
    int screenPos = 0;
    int j = 0;
    int t = 0;
    int spaceBetween = 0;    
    int lineCount = 0;
    unsigned char specialWordColor = COLOR_LIGHT_GRAY;   
    unsigned short tabAttrib = 0;
    unsigned short charAttrib = 0;
    char lineCounterBufferTemp[16];
    char *c = NULL;
    Node *currentNode = NULL;
    Line *line = NULL;

    memset(lineCounterBufferTemp, '\0', 16);

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
        currentNode = currentNode->next;
        lineCount++;
    }

    /* End pre-scan */
    
    for(y = y1; y <= y2; y++){
        if(currentNode){
            line = (Line *)currentNode->data;
            linePos = 0;
            screenX = x1;

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
                    if(linePos + (int)file->scrollX < (int)line->length){
                        c = &line->buffer[linePos + file->scrollX];                
                        
                        if(*c == '\t'){
                            /// UGLYT TEST
                            for(t=0; t < 4 && screenX <= x2; t++){
                                if (
                                    dw_isCharSelected(file, lineCount, linePos + file->scrollX) ||
                                    dw_isCharFound(file, lineCount, linePos + file->scrollX)
                                ){
                                    tabAttrib = ((COLOR_LIGHT_GRAY << 4) | (COLOR_BLACK << 8));
                                } else {
                                    tabAttrib = backgroundColor << 4 | ((t==3 && settings.TAB_INDICATOR == true) ? COLOR_DARK_GRAY : foregroundColor);
                                }
                                destBuffer[(y * VIDEO_COLS) + screenX] = ((t==3 && settings.TAB_INDICATOR == true) ? 179 : ' ') | (tabAttrib << 8);
                                screenX++;
                            }
                            linePos++;
                            /* Subtract one because screenX is incremented in the loop logic below or will be incremented in next iteration */
                            /* Actually we just continue and screenX is already at the next position */
                            continue; 
                        } else if (*c == '\r' || *c == '\n') {
                            /* Skip these as we are already on a new line */
                            linePos++;
                            continue;
                        } else {
                            if (
                                dw_isCharSelected(file, lineCount, linePos + file->scrollX) ||
                                dw_isCharFound(file, lineCount, linePos + file->scrollX)
                            ){
                                charAttrib = ((COLOR_LIGHT_GRAY << 4) | (COLOR_BLACK << 8));
                            } else {
                                charAttrib = backgroundColor << 4 | (specialWordColor ? specialWordColor : foregroundColor);
                            }
                            destBuffer[screenPos] = *c | (charAttrib << 8);
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
/* This also will take care of reserved word coloring */
void dw_writeBufferEditorFormatted(unsigned short *destBuffer, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, File *file){
    switch(file->ext){
        case FILE_EXTENSION_C:
            dw_c_formatter(destBuffer, x1,y1,x2,y2,foregroundColor,backgroundColor,file);
            break;
        case FILE_EXTENSION_PYTHON:
        case FILE_EXTENSION_JS:
        case FILE_EXTENSION_TXT:
        default:
            dw_txt_formatter(destBuffer, x1,y1,x2,y2,foregroundColor,backgroundColor,file);
    }

    return;
}

// SINGLE LINE FORMATTING ===========================================

// TODO: IMPLEMENT FORMATTING
void dw_c_lineFormatter(
	unsigned short *destBuffer, 
	int x, 
	int y,
	size_t destWidth,
	char *srcBuffer
){
	unsigned char tjump=0;
	unsigned short *start = NULL;
	char *commentStartPtr = NULL;
	unsigned char fgcolor = COLOR_LIGHT_GRAY;
	unsigned char bgcolor = COLOR_BLACK;

	if(!destBuffer) return;
	if(!srcBuffer) return;

	// Check if it starts with a comment somewher
	commentStartPtr = strstr(srcBuffer,"//");

	start = destBuffer + ( y * destWidth) + x;

	while(*srcBuffer != '\0'){
		// Check if current character is a comment or not
		if(commentStartPtr && srcBuffer >= commentStartPtr)
			fgcolor = settings.clang_colors[DW_RESWORD_COMMENT];

		switch(*srcBuffer){
			case '\t':
				tjump=0;
				while(tjump<3){
					*(++start) = 
						(unsigned short) ((unsigned char) ' ' | 
						((bgcolor << 4 | fgcolor) << 8));		
					
					tjump++;
				};

				// TAB INDICATOR
				*(start) = 
						(unsigned short) ((unsigned char) 
						((settings.TAB_INDICATOR == true) ? 179 : ' ') | 
						((bgcolor << 4 | COLOR_DARK_GRAY) << 8));
						
						
					
				
				break;
			default:
				*start = 
					(unsigned short) ((unsigned char) *srcBuffer | 
					((bgcolor << 4 | fgcolor) << 8));
		}
		srcBuffer++;
		start++;
	};

	return;
}

// TODO: IMPLEMENT FORMATTING
void dw_txt_lineFormatter(
	unsigned short *destBuffer, 
	int x, 
	int y, 
	size_t destWidth,
	char *srcBuffer
){
	unsigned char tjump=0;
	unsigned short *start = NULL;

	if(!destBuffer) return;
	if(!srcBuffer) return;

	start = destBuffer + ( y * destWidth) + x;

	while(*srcBuffer != '\0'){
		switch(*srcBuffer){
			case '\t':
				tjump=0;
				while(tjump<3){
					*(++start) = 
						(unsigned short) ((unsigned char) ' ' | 
						((COLOR_BLACK << 4 | COLOR_LIGHT_GRAY) << 8));		
					
					tjump++;
				};
				
				break;
			default:
				*start = 
					(unsigned short) ((unsigned char) *srcBuffer | 
					((COLOR_BLACK << 4 | COLOR_LIGHT_GRAY) << 8));
		}
		srcBuffer++;
		start++;
	};

	return;
}

void dw_copyFormatted(
	unsigned short *destBuffer, 
	int x, 
	int y,  
	size_t destWidth,
	char *srcBuffer,
	File *file
){
	switch(file->ext){
        case FILE_EXTENSION_C:
            dw_c_lineFormatter(
				destBuffer, 
				x,
				y,
				destWidth,
				srcBuffer
			);
            break;
        case FILE_EXTENSION_PYTHON:
        case FILE_EXTENSION_JS:
        case FILE_EXTENSION_TXT:
        default:
			dw_txt_lineFormatter(
				destBuffer, 
				x,
				y,
				destWidth,
				srcBuffer
			);
    }

    return;

}

// RENDER EVENT REQUEST-DISPATCH=====================================

void dw_requestRenderEvent(RenderType renderType){
    if(dw_renderEventType < renderType)
        dw_renderEventType = renderType;

    dw_renderEvent = true;
}

// EVENT DISPATCHER
void dw_renderEventDispatcher(){
    if(ed_globalAuxTimer > 0){
        ed_checkStatusBarMessage();
    }

	if(dw_renderEvent == true){
		switch (dw_renderEventType){
			case DW_RENDER_CURSOR:
				ed_updateCursor();
                dw_renderEventType = DW_RENDER_NONE;
				dw_renderEvent = false;
				hal_vid_refresh();
				return;
			case DW_RENDER_LINE:
				ed_renderCurrentLine();
                dw_renderEventType = DW_RENDER_NONE;
				dw_renderEvent = false;
                hal_vid_refresh();
				return;
            case DW_RENDER_STATUSBAR:
		        ed_statusBar();	
                dw_renderEventType = DW_RENDER_NONE;
                dw_renderEvent = false;
                hal_vid_refresh();
                return;
			case DW_RENDER_SEARCH:
				ed_updateWindow(currentWorkspace);
				ed_drawSearchTool();
				break;
			case DW_RENDER_WINDOW:
				ed_updateWindow(currentWorkspace);
				break;
			case DW_RENDER_UI:
				ed_updateWindow(currentWorkspace);
				break;
			case DW_RENDER_ALL:
                ed_renderWindows(currentWorkspace);
                	
				break;
			default:
				break;
			}

        ed_statusBar();

		if(settings.DEBUG == true) 
			t_drawDebugger();

		if(f_onFileNavigation== true) 
			f_drawFileNavDialog();

		ed_updateCursor();
		hal_vid_refresh();
		
		dw_renderEvent = false;
        dw_renderEventType = DW_RENDER_NONE;
	}

}