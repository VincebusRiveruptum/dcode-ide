#include "../../../hal/hal_inp.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdarg.h>
#include <sys/time.h>
#include <linux/input.h>

#define INP_KEY_RELEASE_TIMEOUT 400 // ms
#define FIFO_SIZE 256

/* Keyboard State Globals */
volatile unsigned char inp_state[256];
volatile unsigned char inp_pressed[256];
volatile unsigned char inp_released[256];
volatile unsigned char inp_justPressed[256];
volatile unsigned char inp_justPressedPending[256];

/* FIFO Buffer for standard input */
static char fifo_buf[FIFO_SIZE];
static int fifo_head = 0;
static int fifo_tail = 0;

/* Linux input device globals */
static int keyboard_fd = -1;
static char keyboard_dev_path[256] = "";
static long long last_input_time = 0;
static struct termios orig_termios;
static int raw_mode_active = 0;

/* Forward Declarations */
static void press_key(unsigned char key);
static void fifo_push(char c);

/* Time Utility helper */
static long long get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/* State tracker helper */
static void press_key(unsigned char key) {
    inp_pressed[key] = 1;
    inp_justPressedPending[key] = 1;
}

/* FIFO Helpers */
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

/* Dynamically scan /proc/bus/input/devices to find keyboard handler */
static void find_keyboard_device(void) {
    FILE *f = fopen("/proc/bus/input/devices", "r");
    if (!f) {
        strcpy(keyboard_dev_path, "/dev/input/event0");
        return;
    }

    char line[512];
    char name[256] = "";
    char handlers[256] = "";
    int is_kbd = 0;

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == 'N') {
            char *p = strchr(line, '"');
            if (p) {
                strcpy(name, p + 1);
                char *end = strrchr(name, '"');
                if (end) *end = '\0';
            }
        } else if (line[0] == 'H') {
            strcpy(handlers, line + 12); // Skip "H: Handlers="
        } else if (line[0] == '\n' || line[0] == '\r') {
            if (strstr(handlers, "kbd") && strstr(handlers, "event")) {
                char *ev = strstr(handlers, "event");
                int ev_num = 0;
                if (sscanf(ev, "event%d", &ev_num) == 1) {
                    sprintf(keyboard_dev_path, "/dev/input/event%d", ev_num);
                    is_kbd = 1;
                    break;
                }
            }
            name[0] = '\0';
            handlers[0] = '\0';
        }
    }
    fclose(f);

    if (!is_kbd) {
        strcpy(keyboard_dev_path, "/dev/input/event0");
    }
}

/* Maps hardware Linux kernel key codes to internal HAL key codes */
static unsigned char map_linux_code_to_hal(unsigned short code) {
    if (code < 89) {
        return (unsigned char)code;
    }
    switch (code) {
        case KEY_HOME:     return HAL_KEY_HOME;
        case KEY_UP:       return HAL_KEY_UP;
        case KEY_PAGEUP:   return HAL_KEY_PAGEUP;
        case KEY_LEFT:     return HAL_KEY_LEFT;
        case KEY_RIGHT:    return HAL_KEY_RIGHT;
        case KEY_END:      return HAL_KEY_END;
        case KEY_DOWN:     return HAL_KEY_DOWN;
        case KEY_PAGEDOWN: return HAL_KEY_PAGEDOWN;
        case KEY_INSERT:   return HAL_KEY_INSERT;
        case KEY_DELETE:   return HAL_KEY_DELETE;
        case KEY_KPENTER:  return HAL_KEY_KP_ENTER;
        case KEY_RIGHTCTRL:return HAL_KEY_RCTRL;
        case KEY_KPSLASH:  return HAL_KEY_KP_DIVIDE;
        case KEY_RIGHTALT: return HAL_KEY_RALT;
        default:           return 0;
    }
}

/* Polling the event device file */
static void poll_keyboard_device(void) {
    if (keyboard_fd < 0) return;

    struct input_event evs[64];
    int rd = read(keyboard_fd, evs, sizeof(evs));
    if (rd > 0) {
        int n_events = rd / sizeof(struct input_event);
        int i;
        for (i = 0; i < n_events; i++) {
            if (evs[i].type == EV_KEY) {
                unsigned char hal_key = map_linux_code_to_hal(evs[i].code);
                if (hal_key == 0) continue;

                if (evs[i].value == 1) { // Press
                    inp_state[hal_key] = 1;
                    press_key(hal_key);

                    // Push special action keys to FIFO queue
                    if (hal_key == HAL_KEY_UP) { fifo_push(0); fifo_push(HAL_KEY_UP); }
                    else if (hal_key == HAL_KEY_DOWN) { fifo_push(0); fifo_push(HAL_KEY_DOWN); }
                    else if (hal_key == HAL_KEY_LEFT) { fifo_push(0); fifo_push(HAL_KEY_LEFT); }
                    else if (hal_key == HAL_KEY_RIGHT) { fifo_push(0); fifo_push(HAL_KEY_RIGHT); }
                    else if (hal_key == HAL_KEY_HOME) { fifo_push(0); fifo_push(HAL_KEY_HOME); }
                    else if (hal_key == HAL_KEY_END) { fifo_push(0); fifo_push(HAL_KEY_END); }
                    else if (hal_key == HAL_KEY_PAGEUP) { fifo_push(0); fifo_push(HAL_KEY_PAGEUP); }
                    else if (hal_key == HAL_KEY_PAGEDOWN) { fifo_push(0); fifo_push(HAL_KEY_PAGEDOWN); }
                    else if (hal_key == HAL_KEY_INSERT) { fifo_push(0); fifo_push(HAL_KEY_INSERT); }
                    else if (hal_key == HAL_KEY_DELETE) { fifo_push(0); fifo_push(HAL_KEY_DELETE); }
                    else if (hal_key == HAL_KEY_ESC) { fifo_push(HAL_CHAR_ESCAPE); }
                    else if (hal_key == HAL_KEY_BACKSPACE) { fifo_push(HAL_CHAR_BACKSPACE); }
                    else if (hal_key == HAL_KEY_ENTER) { fifo_push(HAL_CHAR_ENTER); }
                    else if (hal_key == HAL_KEY_TAB) { fifo_push(HAL_CHAR_TAB); }
                    else if (hal_key == HAL_KEY_SPACE) { fifo_push(HAL_CHAR_SPACE); }

                } else if (evs[i].value == 0) { // Release
                    inp_state[hal_key] = 0;
                    inp_released[hal_key] = 1;
                } else if (evs[i].value == 2) { // Repeat
                    press_key(hal_key);
                    if (hal_key == HAL_KEY_UP) { fifo_push(0); fifo_push(HAL_KEY_UP); }
                    else if (hal_key == HAL_KEY_DOWN) { fifo_push(0); fifo_push(HAL_KEY_DOWN); }
                    else if (hal_key == HAL_KEY_LEFT) { fifo_push(0); fifo_push(HAL_KEY_LEFT); }
                    else if (hal_key == HAL_KEY_RIGHT) { fifo_push(0); fifo_push(HAL_KEY_RIGHT); }
                    else if (hal_key == HAL_KEY_HOME) { fifo_push(0); fifo_push(HAL_KEY_HOME); }
                    else if (hal_key == HAL_KEY_END) { fifo_push(0); fifo_push(HAL_KEY_END); }
                    else if (hal_key == HAL_KEY_PAGEUP) { fifo_push(0); fifo_push(HAL_KEY_PAGEUP); }
                    else if (hal_key == HAL_KEY_PAGEDOWN) { fifo_push(0); fifo_push(HAL_KEY_PAGEDOWN); }
                    else if (hal_key == HAL_KEY_INSERT) { fifo_push(0); fifo_push(HAL_KEY_INSERT); }
                    else if (hal_key == HAL_KEY_DELETE) { fifo_push(0); fifo_push(HAL_KEY_DELETE); }
                    else if (hal_key == HAL_KEY_ESC) { fifo_push(HAL_CHAR_ESCAPE); }
                    else if (hal_key == HAL_KEY_BACKSPACE) { fifo_push(HAL_CHAR_BACKSPACE); }
                    else if (hal_key == HAL_KEY_ENTER) { fifo_push(HAL_CHAR_ENTER); }
                    else if (hal_key == HAL_KEY_TAB) { fifo_push(HAL_CHAR_TAB); }
                    else if (hal_key == HAL_KEY_SPACE) { fifo_push(HAL_CHAR_SPACE); }
                }
            }
        }
    }
}

/* non-blocking stdin reader with trailing byte timeouts */
static int read_stdin_timeout(char *buf, int max_len, int timeout_ms) {
    int num_read = 0;
    int r = read(0, &buf[0], 1);
    if (r <= 0) {
        return 0; // immediate return if no input
    }
    num_read = 1;

    struct pollfd pfd;
    pfd.fd = 0;
    pfd.events = POLLIN;
    
    while (num_read < max_len) {
        int ret = poll(&pfd, 1, timeout_ms);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            r = read(0, &buf[num_read], 1);
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

/* Process stdin - raw text input mode (discards ANSI escape sequences) */
static void poll_stdin_raw(void) {
    char buf[64];
    int n = read_stdin_timeout(buf, 64, 10);
    if (n > 0) {
        last_input_time = get_time_ms();
        int i = 0;
        while (i < n) {
            if (buf[i] == 27) { // ESC sequence beginning
                if (i + 1 < n) {
                    if (buf[i+1] == '[' || buf[i+1] == 'O') {
                        int seq_end = i + 2;
                        while (seq_end < n && 
                               !(buf[seq_end] >= 'A' && buf[seq_end] <= 'Z') && 
                               !(buf[seq_end] >= 'a' && buf[seq_end] <= 'z') && 
                               buf[seq_end] != '~') {
                            seq_end++;
                        }
                        i = seq_end + 1;
                        continue;
                    } else {
                        i += 2;
                        continue;
                    }
                }
                i++;
            } else {
                char c = buf[i];
                if (c > 32 && c != 127) {
                    fifo_push(c);
                }
                i++;
            }
        }
    }
}

/* Process stdin - fallback emulation mode (parses escape codes manually) */
static void poll_stdin_fallback(void) {
    char buf[64];
    int n = read_stdin_timeout(buf, 64, 10);
    if (n > 0) {
        last_input_time = get_time_ms();
        inp_state[HAL_KEY_LALT] = 0;
        inp_state[HAL_KEY_LSHIFT] = 0;
        inp_state[HAL_KEY_LCTRL] = 0;
        int i = 0;
        while (i < n) {
            if (buf[i] == 27) { // ESC sequence
                if (i + 1 < n) {
                    if (buf[i+1] == 27) {
                        inp_state[HAL_KEY_LALT] = 1;
                        press_key(HAL_KEY_LALT);
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
                            if (buf[i+2] == 'A') { fifo_push(0); fifo_push(HAL_KEY_UP); inp_state[HAL_KEY_UP] = 1; press_key(HAL_KEY_UP); }
                            else if (buf[i+2] == 'B') { fifo_push(0); fifo_push(HAL_KEY_DOWN); inp_state[HAL_KEY_DOWN] = 1; press_key(HAL_KEY_DOWN); }
                            else if (buf[i+2] == 'C') { fifo_push(0); fifo_push(HAL_KEY_RIGHT); inp_state[HAL_KEY_RIGHT] = 1; press_key(HAL_KEY_RIGHT); }
                            else if (buf[i+2] == 'D') { fifo_push(0); fifo_push(HAL_KEY_LEFT); inp_state[HAL_KEY_LEFT] = 1; press_key(HAL_KEY_LEFT); }
                            else if (buf[i+2] == 'H') { fifo_push(0); fifo_push(HAL_KEY_HOME); inp_state[HAL_KEY_HOME] = 1; press_key(HAL_KEY_HOME); }
                            else if (buf[i+2] == 'F') { fifo_push(0); fifo_push(HAL_KEY_END); inp_state[HAL_KEY_END] = 1; press_key(HAL_KEY_END); }
                            else if (strncmp(&buf[i+2], "5~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_PAGEUP); inp_state[HAL_KEY_PAGEUP] = 1; press_key(HAL_KEY_PAGEUP); }
                            else if (strncmp(&buf[i+2], "6~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_PAGEDOWN); inp_state[HAL_KEY_PAGEDOWN] = 1; press_key(HAL_KEY_PAGEDOWN); }
                            else if (strncmp(&buf[i+2], "2~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_INSERT); inp_state[HAL_KEY_INSERT] = 1; press_key(HAL_KEY_INSERT); }
                            else if (strncmp(&buf[i+2], "3~", seq_end - (i+2) + 1) == 0) { fifo_push(0); fifo_push(HAL_KEY_DELETE); inp_state[HAL_KEY_DELETE] = 1; press_key(HAL_KEY_DELETE); }
                            else if (strncmp(&buf[i+2], "11~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F1] = 1; press_key(HAL_KEY_F1); }
                            else if (strncmp(&buf[i+2], "12~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F2] = 1; press_key(HAL_KEY_F2); }
                            else if (strncmp(&buf[i+2], "20~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F9] = 1; press_key(HAL_KEY_F9); }
                            else if (strncmp(&buf[i+2], "23~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F11] = 1; press_key(HAL_KEY_F11); }
                            else if (strncmp(&buf[i+2], "24~", seq_end - (i+2) + 1) == 0) { inp_state[HAL_KEY_F12] = 1; press_key(HAL_KEY_F12); }
                            else if (buf[i+2] == '1' && buf[i+3] == ';') {
                                char mod = buf[i+4];
                                char key = buf[i+5];
                                if (mod == '2') { // Shift
                                    inp_state[HAL_KEY_LSHIFT] = 1;
                                    press_key(HAL_KEY_LSHIFT);
                                    if (key == 'A') { fifo_push(0); fifo_push(HAL_KEY_UP); inp_state[HAL_KEY_UP] = 1; press_key(HAL_KEY_UP); }
                                    else if (key == 'B') { fifo_push(0); fifo_push(HAL_KEY_DOWN); inp_state[HAL_KEY_DOWN] = 1; press_key(HAL_KEY_DOWN); }
                                    else if (key == 'C') { fifo_push(0); fifo_push(HAL_KEY_RIGHT); inp_state[HAL_KEY_RIGHT] = 1; press_key(HAL_KEY_RIGHT); }
                                    else if (key == 'D') { fifo_push(0); fifo_push(HAL_KEY_LEFT); inp_state[HAL_KEY_LEFT] = 1; press_key(HAL_KEY_LEFT); }
                                } else if (mod == '3') { // Alt
                                    inp_state[HAL_KEY_LALT] = 1;
                                    press_key(HAL_KEY_LALT);
                                    if (key == 'A') { inp_state[HAL_KEY_UP] = 1; press_key(HAL_KEY_UP); }
                                    else if (key == 'B') { inp_state[HAL_KEY_DOWN] = 1; press_key(HAL_KEY_DOWN); }
                                } else if (mod == '5') { // Ctrl
                                    inp_state[HAL_KEY_LCTRL] = 1;
                                    press_key(HAL_KEY_LCTRL);
                                    if (key == 'C') { inp_state[HAL_KEY_RIGHT] = 1; press_key(HAL_KEY_RIGHT); }
                                    else if (key == 'D') { inp_state[HAL_KEY_LEFT] = 1; press_key(HAL_KEY_LEFT); }
                                }
                            }
                            i = seq_end + 1;
                            continue;
                        }
                    } else if (buf[i+1] == 'O') {
                        if (buf[i+2] == 'P') { inp_state[HAL_KEY_F1] = 1; press_key(HAL_KEY_F1); }
                        else if (buf[i+2] == 'Q') { inp_state[HAL_KEY_F2] = 1; press_key(HAL_KEY_F2); }
                        else if (buf[i+2] == 'R') { inp_state[HAL_KEY_F3] = 1; press_key(HAL_KEY_F3); }
                        else if (buf[i+2] == 'S') { inp_state[HAL_KEY_F4] = 1; press_key(HAL_KEY_F4); }
                        i += 3;
                        continue;
                    } else if (buf[i+1] == '\t' || buf[i+1] == 's' || buf[i+1] == 'S') {
                        inp_state[HAL_KEY_LALT] = 1;
                        press_key(HAL_KEY_LALT);
                        inp_state[HAL_KEY_LSHIFT] = 1;
                        press_key(HAL_KEY_LSHIFT);
                        i += 2;
                        continue;
                    }
                }
                fifo_push(HAL_CHAR_ESCAPE);
                inp_state[HAL_KEY_ESC] = 1;
                press_key(HAL_KEY_ESC);
                i++;
            } else {
                char c = buf[i];
                if (c == 127 || c == 8) {
                    fifo_push(HAL_CHAR_BACKSPACE);
                    inp_state[HAL_KEY_BACKSPACE] = 1;
                    press_key(HAL_KEY_BACKSPACE);
                } else if (c == '\n' || c == '\r') {
                    fifo_push(HAL_CHAR_ENTER);
                    inp_state[HAL_KEY_ENTER] = 1;
                    press_key(HAL_KEY_ENTER);
                } else if (c == '\t') {
                    fifo_push(HAL_CHAR_TAB);
                    inp_state[HAL_KEY_TAB] = 1;
                    press_key(HAL_KEY_TAB);
                } else if (c == ' ') {
                    fifo_push(HAL_CHAR_SPACE);
                    inp_state[HAL_KEY_SPACE] = 1;
                    press_key(HAL_KEY_SPACE);
                } else if ((c >= 1 && c <= 26) || c == 28) {
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
                        press_key(HAL_KEY_LCTRL);
                        inp_state[mapped_key] = 1;
                        press_key(mapped_key);
                        if (mapped_key == HAL_KEY_S) {
                            inp_state[HAL_KEY_LSHIFT] = 1;
                            press_key(HAL_KEY_LSHIFT);
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

/* Stdin polling router */
static void poll_stdin(void) {
    if (keyboard_fd >= 0) {
        poll_stdin_raw();
    } else {
        poll_stdin_fallback();
    }
}

/* HAL Key API implementation */
void hal_inp_initKeyboard(void) {
    int i;
    for (i = 0; i < 256; i++) {
        inp_state[i] = 0;
        inp_pressed[i] = 0;
        inp_released[i] = 0;
        inp_justPressed[i] = 0;
        inp_justPressedPending[i] = 0;
    }
    
    last_input_time = get_time_ms();
    
    find_keyboard_device();
    keyboard_fd = open(keyboard_dev_path, O_RDONLY | O_NONBLOCK);
    
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
    if (keyboard_fd >= 0) {
        close(keyboard_fd);
        keyboard_fd = -1;
    }
    if (raw_mode_active) {
        tcsetattr(0, TCSANOW, &orig_termios);
        raw_mode_active = 0;
    }
}

void hal_inp_updateKeyboard(void) {
    int i;
    long long current_time = get_time_ms();
    
    /* 1. CPU optimization: block up to 50ms if FIFO is empty to avoid high CPU usage */
    if (fifo_empty()) {
        struct pollfd fds[2];
        int nfds = 1;
        fds[0].fd = 0; // stdin
        fds[0].events = POLLIN;
        if (keyboard_fd >= 0) {
            fds[1].fd = keyboard_fd;
            fds[1].events = POLLIN;
            nfds = 2;
        }
        poll(fds, nfds, 50);
    }

    /* 2. Poll the keyboard raw event file */
    poll_keyboard_device();

    /* 3. Latch and update frame keyboard states */
    for (i = 0; i < 256; i++) {
        if (keyboard_fd < 0) {
            if (i != HAL_KEY_LALT && i != HAL_KEY_LCTRL && i != HAL_KEY_LSHIFT) {
                inp_state[i] = 0;
            }
        }
        inp_justPressed[i] = inp_justPressedPending[i];
        inp_justPressedPending[i] = 0;
        inp_released[i] = 0;
    }

    /* 4. Release modifier keys if inactive under fallback mode */
    if (keyboard_fd < 0) {
        if (current_time - last_input_time > INP_KEY_RELEASE_TIMEOUT) {
            inp_state[HAL_KEY_LALT] = 0;
            inp_state[HAL_KEY_LSHIFT] = 0;
            inp_state[HAL_KEY_LCTRL] = 0;
        }
    }

    /* 5. Process standard input queue (stdin) */
    poll_stdin();
}

void hal_inp_clearKeyboardBuffer(void) {
    int i;
    for (i = 0; i < 256; i++) {
        inp_pressed[i] = 0;
        inp_released[i] = 0;
        inp_justPressed[i] = 0;
        inp_justPressedPending[i] = 0;
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
        if (inp_state[key] == 0 && inp_justPressed[key] == 0) {
            allDown = 0;
        }
        if (inp_justPressed[key] > 0) {
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
