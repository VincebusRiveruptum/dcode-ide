#include "../../../hal/hal_inp.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdarg.h>

volatile unsigned char inp_state[256];
volatile unsigned char inp_pressed[256];
volatile unsigned char inp_released[256];
volatile unsigned char inp_justPressed[256];

static struct termios orig_termios;
static int raw_mode_active = 0;

#define FIFO_SIZE 256
static char fifo_buf[FIFO_SIZE];
static int fifo_head = 0;
static int fifo_tail = 0;

static void fifo_push(char c) {
    int next = (fifo_tail + 1) % FIFO_SIZE;
    if (next != fifo_head) {
        fifo_buf[fifo_tail] = c;
        fifo_tail = next;
    }
}

static char fifo_pop(void) {
    if (fifo_head == fifo_tail) return 0;
    char c = fifo_buf[fifo_head];
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
    return c;
}

static bool fifo_empty(void) {
    return fifo_head == fifo_tail;
}

static int read_stdin_timeout(char *buf, int max_len, int timeout_ms) {
    struct pollfd pfd;
    pfd.fd = 0; // stdin
    pfd.events = POLLIN;
    
    int num_read = 0;
    while (num_read < max_len) {
        int ret = poll(&pfd, 1, num_read == 0 ? 0 : timeout_ms);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            int r = read(0, &buf[num_read], 1);
            if (r > 0) {
                num_read++;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    return num_read;
}

static void poll_stdin(void) {
    char buf[64];
    int n = read_stdin_timeout(buf, 64, 10);
    if (n > 0) {
        inp_state[HAL_KEY_LALT] = 0;
        inp_state[HAL_KEY_LSHIFT] = 0;
        inp_state[HAL_KEY_LCTRL] = 0;
        int i = 0;
        while (i < n) {
            if (buf[i] == 27) { // ESC
                if (i + 1 < n) {
                    if (buf[i+1] == 27) {
                        inp_state[HAL_KEY_LALT] = 1;
                        inp_pressed[HAL_KEY_LALT] = 1;
                        i++;
                        continue;
                    }
                    if (buf[i+1] == '[') {
                        int seq_end = i + 2;
                        while (seq_end < n && 
                               !(buf[seq_end] >= 'A' && buf[seq_end] <= 'Z') && 
                               !(buf[seq_end] >= 'a' && buf[seq_end] <= 'z') && 
                               buf[seq_end] != '~') {
                            seq_end++;
                        }
                        if (seq_end < n) {
                            if (buf[i+2] == 'A') { fifo_push(0); fifo_push(HAL_KEY_UP); inp_state[HAL_KEY_UP] = 1; inp_pressed[HAL_KEY_UP] = 1; }
                            else if (buf[i+2] == 'B') { fifo_push(0); fifo_push(HAL_KEY_DOWN); inp_state[HAL_KEY_DOWN] = 1; inp_pressed[HAL_KEY_DOWN] = 1; }
                            else if (buf[i+2] == 'C') { fifo_push(0); fifo_push(HAL_KEY_RIGHT); inp_state[HAL_KEY_RIGHT] = 1; inp_pressed[HAL_KEY_RIGHT] = 1; }
                            else if (buf[i+2] == 'D') { fifo_push(0); fifo_push(HAL_KEY_LEFT); inp_state[HAL_KEY_LEFT] = 1; inp_pressed[HAL_KEY_LEFT] = 1; }
                            else if (buf[i+2] == 'H') { fifo_push(0); fifo_push(HAL_KEY_HOME); inp_state[HAL_KEY_HOME] = 1; inp_pressed[HAL_KEY_HOME] = 1; }
                            else if (buf[i+2] == 'F') { fifo_push(0); fifo_push(HAL_KEY_END); inp_state[HAL_KEY_END] = 1; inp_pressed[HAL_KEY_END] = 1; }
                            else if (strncmp(&buf[i+2], "5~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_PAGEUP); inp_state[HAL_KEY_PAGEUP] = 1; inp_pressed[HAL_KEY_PAGEUP] = 1; }
                            else if (strncmp(&buf[i+2], "6~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_PAGEDOWN); inp_state[HAL_KEY_PAGEDOWN] = 1; inp_pressed[HAL_KEY_PAGEDOWN] = 1; }
                            else if (strncmp(&buf[i+2], "2~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_INSERT); inp_state[HAL_KEY_INSERT] = 1; inp_pressed[HAL_KEY_INSERT] = 1; }
                            else if (strncmp(&buf[i+2], "3~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_DELETE); inp_state[HAL_KEY_DELETE] = 1; inp_pressed[HAL_KEY_DELETE] = 1; }
                            else if (strncmp(&buf[i+2], "11~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F1] = 1; inp_pressed[HAL_KEY_F1] = 1; }
                            else if (strncmp(&buf[i+2], "12~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F2] = 1; inp_pressed[HAL_KEY_F2] = 1; }
                            else if (strncmp(&buf[i+2], "20~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F9] = 1; inp_pressed[HAL_KEY_F9] = 1; }
                            else if (strncmp(&buf[i+2], "23~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F11] = 1; inp_pressed[HAL_KEY_F11] = 1; }
                            else if (strncmp(&buf[i+2], "24~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F12] = 1; inp_pressed[HAL_KEY_F12] = 1; }
                            else if (buf[i+2] == '1' && buf[i+3] == ';') {
                                char mod = buf[i+4];
                                char key = buf[i+5];
                                if (mod == '2') { // Shift
                                    inp_state[HAL_KEY_LSHIFT] = 1;
                                    inp_pressed[HAL_KEY_LSHIFT] = 1;
                                    if (key == 'A') { fifo_push(0); fifo_push(HAL_KEY_UP); inp_state[HAL_KEY_UP] = 1; inp_pressed[HAL_KEY_UP] = 1; }
                                    else if (key == 'B') { fifo_push(0); fifo_push(HAL_KEY_DOWN); inp_state[HAL_KEY_DOWN] = 1; inp_pressed[HAL_KEY_DOWN] = 1; }
                                    else if (key == 'C') { fifo_push(0); fifo_push(HAL_KEY_RIGHT); inp_state[HAL_KEY_RIGHT] = 1; inp_pressed[HAL_KEY_RIGHT] = 1; }
                                    else if (key == 'D') { fifo_push(0); fifo_push(HAL_KEY_LEFT); inp_state[HAL_KEY_LEFT] = 1; inp_pressed[HAL_KEY_LEFT] = 1; }
                                } else if (mod == '3') { // Alt
                                    inp_state[HAL_KEY_LALT] = 1;
                                    inp_pressed[HAL_KEY_LALT] = 1;
                                    if (key == 'A') { inp_state[HAL_KEY_UP] = 1; inp_pressed[HAL_KEY_UP] = 1; }
                                    else if (key == 'B') { inp_state[HAL_KEY_DOWN] = 1; inp_pressed[HAL_KEY_DOWN] = 1; }
                                } else if (mod == '5') { // Ctrl
                                    inp_state[HAL_KEY_LCTRL] = 1;
                                    inp_pressed[HAL_KEY_LCTRL] = 1;
                                    if (key == 'C') { inp_state[HAL_KEY_RIGHT] = 1; inp_pressed[HAL_KEY_RIGHT] = 1; }
                                    else if (key == 'D') { inp_state[HAL_KEY_LEFT] = 1; inp_pressed[HAL_KEY_LEFT] = 1; }
                                }
                            }
                            i = seq_end + 1;
                            continue;
                        }
                    } else if (buf[i+1] == 'O') {
                        if (buf[i+2] == 'P') { inp_state[HAL_KEY_F1] = 1; inp_pressed[HAL_KEY_F1] = 1; }
                        else if (buf[i+2] == 'Q') { inp_state[HAL_KEY_F2] = 1; inp_pressed[HAL_KEY_F2] = 1; }
                        else if (buf[i+2] == 'R') { inp_state[HAL_KEY_F3] = 1; inp_pressed[HAL_KEY_F3] = 1; }
                        else if (buf[i+2] == 'S') { inp_state[HAL_KEY_F4] = 1; inp_pressed[HAL_KEY_F4] = 1; }
                        i += 3;
                        continue;
                    } else if (buf[i+1] == '\t' || buf[i+1] == 's' || buf[i+1] == 'S') {
                        inp_state[HAL_KEY_LALT] = 1;
                        inp_pressed[HAL_KEY_LALT] = 1;
                        inp_state[HAL_KEY_LSHIFT] = 1;
                        inp_pressed[HAL_KEY_LSHIFT] = 1;
                        i += 2;
                        continue;
                    }
                }
                fifo_push(HAL_CHAR_ESCAPE);
                inp_state[HAL_KEY_ESC] = 1;
                inp_pressed[HAL_KEY_ESC] = 1;
                i++;
            } else {
                char c = buf[i];
                if (c == 127 || c == 8) {
                    fifo_push(HAL_CHAR_BACKSPACE);
                    inp_state[HAL_KEY_BACKSPACE] = 1;
                    inp_pressed[HAL_KEY_BACKSPACE] = 1;
                } else if (c == '\n' || c == '\r') {
                    fifo_push(HAL_CHAR_ENTER);
                    inp_state[HAL_KEY_ENTER] = 1;
                    inp_pressed[HAL_KEY_ENTER] = 1;
                } else if (c == '\t') {
                    fifo_push(HAL_CHAR_TAB);
                    inp_state[HAL_KEY_TAB] = 1;
                    inp_pressed[HAL_KEY_TAB] = 1;
                } else if (c == ' ') {
                    fifo_push(HAL_CHAR_SPACE);
                    inp_state[HAL_KEY_SPACE] = 1;
                    inp_pressed[HAL_KEY_SPACE] = 1;
                } else if ((c >= 1 && c <= 26) || c == 28) {
                    // Map Ctrl+Key sequences
                    unsigned char mapped_key = 0;
                    switch (c) {
                        case 1: mapped_key = HAL_KEY_A; break;
                        case 2: mapped_key = HAL_KEY_B; break;
                        case 3: mapped_key = HAL_KEY_C; break;
                        case 4: mapped_key = HAL_KEY_D; break;
                        case 5: mapped_key = HAL_KEY_E; break;
                        case 6: mapped_key = HAL_KEY_F; break;
                        case 7: mapped_key = HAL_KEY_G; break;
                        case 8: mapped_key = HAL_KEY_H; break;
                        case 11: mapped_key = HAL_KEY_K; break;
                        case 12: mapped_key = HAL_KEY_L; break;
                        case 14: mapped_key = HAL_KEY_N; break;
                        case 15: mapped_key = HAL_KEY_O; break;
                        case 16: mapped_key = HAL_KEY_P; break;
                        case 17: mapped_key = HAL_KEY_Q; break;
                        case 18: mapped_key = HAL_KEY_R; break;
                        case 19: mapped_key = HAL_KEY_S; break;
                        case 20: mapped_key = HAL_KEY_T; break;
                        case 21: mapped_key = HAL_KEY_U; break;
                        case 22: mapped_key = HAL_KEY_V; break;
                        case 23: mapped_key = HAL_KEY_W; break;
                        case 24: mapped_key = HAL_KEY_X; break;
                        case 25: mapped_key = HAL_KEY_Y; break;
                        case 26: mapped_key = HAL_KEY_Z; break;
                        case 28: mapped_key = HAL_KEY_BACKSLASH; break;
                    }
                    if (mapped_key != 0) {
                        inp_state[HAL_KEY_LCTRL] = 1;
                        inp_pressed[HAL_KEY_LCTRL] = 1;
                        inp_state[mapped_key] = 1;
                        inp_pressed[mapped_key] = 1;
                        if (mapped_key == HAL_KEY_S) {
                            // Map Ctrl+S to also trigger Ctrl+Shift+S (LSHIFT) for save
                            inp_state[HAL_KEY_LSHIFT] = 1;
                            inp_pressed[HAL_KEY_LSHIFT] = 1;
                        }
                    }
                } else {
                    fifo_push(c);
                }
                i++;
            }
        }
    }
}

void hal_inp_initKeyboard(void) {
    int i;
    for (i = 0; i < 256; i++) {
        inp_state[i] = 0;
        inp_pressed[i] = 0;
        inp_released[i] = 0;
        inp_justPressed[i] = 0;
    }
    
    if (!raw_mode_active) {
        tcgetattr(0, &orig_termios);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO | ISIG);
        raw.c_iflag &= ~(IXON | ICRNL);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(0, TCSANOW, &raw);
        raw_mode_active = 1;
    }
}

void hal_inp_closeKeyboard(void) {
    if (raw_mode_active) {
        tcsetattr(0, TCSANOW, &orig_termios);
        raw_mode_active = 0;
    }
}

void hal_inp_updateKeyboard(void) {
    int i;
    for (i = 0; i < 256; i++) {
        if (i != HAL_KEY_LALT && i != HAL_KEY_LCTRL && i != HAL_KEY_LSHIFT) {
            inp_state[i] = 0;
        }
        inp_justPressed[i] = 0;
        inp_released[i] = 0;
    }

    poll_stdin();
}

void hal_inp_clearKeyboardBuffer(void) {
    int i;
    for (i = 0; i < 256; i++) {
        inp_pressed[i] = 0;
        inp_released[i] = 0;
        inp_justPressed[i] = 0;
    }
    fifo_head = 0;
    fifo_tail = 0;
}

bool hal_inp_isKeyDown(unsigned char key) {
    return inp_state[key] != 0;
}

bool hal_inp_isKeyPressed(unsigned char key) {
    if (inp_pressed[key] > 0) {
        inp_pressed[key]--;
        return true;
    }
    return false;
}

bool hal_inp_isKeyReleased(unsigned char key) {
    if (inp_released[key] > 0) {
        inp_released[key]--;
        return true;
    }
    return false;
}

unsigned char hal_inp_keysPressed(int triggerType, int nKeys, ...) {
    int i;
    int allDown = 1;
    int anyJustPressed = 0;
    unsigned char key;
    va_list args;
    
    va_start(args, nKeys);
    for (i = 0; i < nKeys; i++) {
        key = (unsigned char)va_arg(args, int);
        if (inp_state[key] == 0 && inp_pressed[key] == 0) {
            allDown = 0;
        }
        if (inp_pressed[key] > 0) {
            anyJustPressed = 1;
        }
    }
    va_end(args);

    if (triggerType == HAL_INP_TRIGGER_LEVEL) {
        return (unsigned char)allDown;
    }
    return (unsigned char)(allDown && anyJustPressed);
}

unsigned char hal_inp_keysDown(int nKeys, ...) {
    int i;
    unsigned char key;
    va_list args;
    
    va_start(args, nKeys);
    for (i = 0; i < nKeys; i++) {
        key = (unsigned char)va_arg(args, int);
        if (inp_state[key] == 0) {
            va_end(args);
            return 0;
        }
    }
    va_end(args);
    return 1;
}

void hal_inp_waitForRelease(void) {
    hal_inp_clearKeyboardBuffer();
}

bool hal_inp_kbhit(void) {
    poll_stdin();
    return !fifo_empty();
}

char hal_inp_getch(void) {
    while (fifo_empty()) {
        poll_stdin();
        usleep(5000); // 5ms sleep
    }
    return fifo_pop();
}

void hal_sleep_ms(int ms) {
    usleep(ms * 1000);
}
