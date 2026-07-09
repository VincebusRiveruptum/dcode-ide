#include "../../../hal/hal_vid.h"
#include "../../../core/config/config.h"
#include "../../../core/editor/editor.h"
#include "../../../core/draw/draw.h"
#include <sys/ioctl.h>
#include <unistd.h>

unsigned short *textmemptr = NULL;
unsigned short *editormemptr = NULL;
int v_currentMode = HAL_VID_80X25;

char tempBuffer[HAL_VIDEO_BUFFER_SIZE];

void hal_vid_init(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0 && w.ws_row > 0) {
        VIDEO_COLS = w.ws_col;
        VIDEO_ROWS = w.ws_row;
    } else {
        VIDEO_COLS = 80;
        VIDEO_ROWS = 25;
    }

    textmemptr = (unsigned short *)malloc(VIDEO_COLS * VIDEO_ROWS * sizeof(unsigned short));
    editormemptr = (unsigned short *)malloc(VIDEO_COLS * VIDEO_ROWS * sizeof(unsigned short));
    hal_vid_clearBuffer(textmemptr);
    hal_vid_clearBuffer(editormemptr);
    
    // Hide cursor and clear terminal screen
    printf("\033[?25l\033[2J");
    fflush(stdout);
}

void hal_vid_close(void) {
    if (textmemptr) {
        free(textmemptr);
        textmemptr = NULL;
    }
    if (editormemptr) {
        free(editormemptr);
        editormemptr = NULL;
    }
    // Show cursor, reset colors, clear screen
    printf("\033[0m\033[2J\033[?25h\033[H");
    fflush(stdout);
}

void hal_vid_clearBuffer(unsigned short *buffer) {
    int i;
    int size = VIDEO_COLS * VIDEO_ROWS;
    for (i = 0; i < size; i++) {
        buffer[i] = ' ' | (0x07 << 8); // Space with light gray on black
    }
}

unsigned char hal_vid_setVideoMode(unsigned char mode, unsigned char show_msg) {
    struct winsize w;
    (void)show_msg;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0 && w.ws_row > 0) {
        VIDEO_COLS = w.ws_col;
        VIDEO_ROWS = w.ws_row;
    } else {
        VIDEO_COLS = 80;
        VIDEO_ROWS = 25;
    }
    
    v_currentMode = mode;
    // Clear screen to match mode change
    printf("\033[2J");
    fflush(stdout);
    return mode;
}

void hal_vid_cycleVideoModes(void) {
    v_currentMode++;
    if (v_currentMode > 8) v_currentMode = 1;
    hal_vid_setVideoMode(v_currentMode, HAL_SHOW_MSG);
}

void hal_vid_set25Lines(void) { hal_vid_setVideoMode(HAL_VID_80X25, HAL_NO_MSG); }
void hal_vid_set43Lines(void) { hal_vid_setVideoMode(HAL_VID_80X43, HAL_NO_MSG); }
void hal_vid_set50Lines(void) { hal_vid_setVideoMode(HAL_VID_80X50, HAL_NO_MSG); }
void hal_vid_set80x60(void) {}
void hal_vid_set132x25(void) {}
void hal_vid_set132x43(void) {}
void hal_vid_set132x50(void) { hal_vid_setVideoMode(HAL_VID_132X50, HAL_NO_MSG); }
void hal_vid_set132x60(void) { hal_vid_setVideoMode(HAL_VID_132X60, HAL_NO_MSG); }

unsigned short hal_vid_getVideoBufferSize(void) {
    return VIDEO_COLS * VIDEO_ROWS;
}

static void print_translated_char(unsigned char c) {
    switch (c) {
        // Blocks
        case 0xDB: printf("█"); break;
        case 0xB0: printf("░"); break;
        case 0xB1: printf("▒"); break;
        case 0xB2: printf("▓"); break;
        // Single borders
        case 179: printf("│"); break;
        case 196: printf("─"); break;
        case 218: printf("┌"); break;
        case 191: printf("┐"); break;
        case 192: printf("└"); break;
        case 217: printf("┘"); break;
        case 195: printf("├"); break;
        case 180: printf("┤"); break;
        case 193: printf("┴"); break;
        case 194: printf("┬"); break;
        case 197: printf("┼"); break;
        // Double borders
        case 186: printf("║"); break;
        case 205: printf("═"); break;
        case 201: printf("╔"); break;
        case 187: printf("╗"); break;
        case 200: printf("╚"); break;
        case 188: printf("╝"); break;
        case 204: printf("╠"); break;
        case 185: printf("╣"); break;
        case 202: printf("╩"); break;
        case 203: printf("╦"); break;
        case 206: printf("╬"); break;
        default:
            if (c >= 32 && c < 127) {
                putchar(c);
            } else if (c == 0) {
                putchar(' ');
            } else {
                putchar(' ');
            }
            break;
    }
}

void hal_vid_refresh(void) {
    static const int fg_ansi[] = { 30, 34, 32, 36, 31, 35, 33, 37, 90, 94, 92, 96, 91, 95, 93, 97 };
    static const int bg_ansi[] = { 40, 44, 42, 46, 41, 45, 43, 47, 100, 104, 102, 106, 101, 105, 103, 107 };
    
    int y, x;
    int last_fg = -1;
    int last_bg = -1;
    struct winsize w;

    // Check if terminal size has changed
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0 && w.ws_row > 0) {
        if (w.ws_col != VIDEO_COLS || w.ws_row != VIDEO_ROWS) {
            VIDEO_COLS = w.ws_col;
            VIDEO_ROWS = w.ws_row;
            
            textmemptr = (unsigned short *)realloc(textmemptr, VIDEO_COLS * VIDEO_ROWS * sizeof(unsigned short));
            editormemptr = (unsigned short *)realloc(editormemptr, VIDEO_COLS * VIDEO_ROWS * sizeof(unsigned short));
            
            hal_vid_clearBuffer(textmemptr);
            hal_vid_clearBuffer(editormemptr);
            
            printf("\033[2J");
            fflush(stdout);
            
            dw_renderEvent = true;
            dw_renderEventType = DW_RENDER_ALL;
            return;
        }
    }

    // Hide cursor during character updates to prevent flickering
    printf("\033[?25l");

    // Move cursor to top left
    printf("\033[H");

    for (y = 0; y < VIDEO_ROWS; y++) {
        for (x = 0; x < VIDEO_COLS; x++) {
            unsigned short cell = textmemptr[y * VIDEO_COLS + x];
            unsigned char c = cell & 0xFF;
            unsigned char attr = (cell >> 8) & 0xFF;
            int fg = attr & 0x0F;
            int bg = (attr >> 4) & 0x0F;

            if (fg != last_fg || bg != last_bg) {
                printf("\033[%d;%dm", fg_ansi[fg], bg_ansi[bg]);
                last_fg = fg;
                last_bg = bg;
            }
            print_translated_char(c);
        }
        // Move to next line or print newline
        if (y < VIDEO_ROWS - 1) {
            printf("\r\n");
        }
    }

    // Set cursor position and show cursor
    printf("\033[%d;%dH\033[?25h", currentCursorY + 1, currentCursorX + 1);
    fflush(stdout);
}

void hal_vid_putCursor(unsigned char x, unsigned char y) {
    currentCursorX = x;
    currentCursorY = y;
}
