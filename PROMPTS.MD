# DCode IDE Development Diary

### Vincebus Riveruptum, 2026

# 17.01.2026

I did previously explain that this is a DOS 32-bit custom programming IDE project, why i decided to make one? Its due that the existing DOS editors are completely outdated, are not friendly for modern users that are used to vscode shortcuts, window, identing, formatting, etc. So, i plan to make a small one by myself. This will render everything in text mode and will be able to switch between 25 and 43 lines if the user desires , this will use TUI or Text UI as its enough for old DOS machines over 386.

After reading a bit, i found that i have to use VGA MODE 3 and its features such as background, foreground color, text blinking, etc. it wouldn be too hard as i can access directly to the video buffer on a specific address, and in 32 bit mode is flat. Howver seems like that VGA MODE 3 only handles 25 lines, it is that true?

Also, i plan on reusing the DEPS\INPUT routines i've made previously for handling keyboard input, this however uses an ISR, could be this appropiate? This will be able to fully control the keyboard but, if i want to type text directly i have to tinker a bit on the current routine code as just detects keystrokes, i need to somehow bypass the actual pressed key value instead of the scan code, then print it on screen on the positioned cursor.

After discussing with the AI, i got informed that i'm able to use 43 or 50 lines in VGA MODE 3 so this is not an issue, however i need to know how to implement this.

Also, it mentioned that the ISR Input implementaion is good for games, but it needs a layer
that translates the scancode to the desired character to be displayed on screen.

Also, i have questions of how to handle the opened text buffer, it will be a struct with metadata and the opened file content, stored as an array of characters. What happens if the text is too large?

Found that the interrupt handling in 32bit mode is done by DOS4GW where it manaages by translating the IVT to protected mode IVT. 

////////////

I got the question, if i do an ISR implementaiton, i also have to do the codepage and keyboard coutry implementation? This could be  unnecesary. Now in this case maybe using DOS keyboard funtions could be better?

# Keyboard interrupts from three levels

## 1. INT 06h - Hardware level keyboard handling

So, at this level we interact directly with the keyboard by reading the scan code emmited by the keyboard
from port 0x60
Other features include :

- Keyboard buffer interaction
- Caps lock, num lock, scroll lock, control.
- End-of-interrupt signal to PIC

## 2. INT 16h - BIOS Level keyboard handling

Layer that abstracts hardware to BIOS level, providing ASCII output from keyboard strokes.

## 3. INT 21h - DOS Level service routines & keyboard handling

This interrupts handle many DOS-level services (File I/O, Memory management, Date/Time, etc), but on this topic we will analyze the keyboard handling.

At this level, int 21H provides codepage translation, STDIN redirection, CTRL + C breaking, line editing, etc.

┌─────────────────────────────────────┐
│     INT 21h - DOS Services          │  ← Highest level (application layer)
│  (Character I/O, Files, Memory)     │
└──────────────┬──────────────────────┘
               │ calls
┌──────────────▼──────────────────────┐
│     INT 16h - BIOS Keyboard         │  ← Middle level (BIOS services)
│  (Key reading, buffer management)   │
└──────────────┬──────────────────────┘
               │ triggered by
┌──────────────▼──────────────────────┐
│     INT 09h - Keyboard Hardware IRQ │  ← Lowest level (hardware ISR)
│  (Scan codes from keyboard port)    │
└─────────────────────────────────────┘
```

I'm getting tired of not being able to write a single line of assembly, i need to do it soon
as possible for me there is an ethic decision where a real programmer solves and engineers solutions
from scratch, using AI does a lot of help but i don't want to overuse it, but at the same time i want
to be more productive and generate code faster, so i need to find a balance... at which point?

If i want to write assembly code i need to know how to do it... i know the basics, there are registers
like:
EAX : AX, AL, AH
EBX : BX, BL, BH
ECX : CX, CL, CH
EDX : DX, DL, DH

Then we have the segment registers:
CS
DS
ES
SS

Some basic instructions:
MOV
ADD
MUL
DIV
SUB
INC
DEC
INT

Port handling instructions:
    IN
    OUT
    
---

# 18.01.2026


### File buffering idea

So, i woke up and started thinking on got the text will be manipulated, i mean, first we open the file, then store it in memory, lets call 
this space as 'file buffer' . However, we need to do double buffering as where we render the text that will be on the screen, lets call this 'video buffer'. Having two buffers will be helpful for many reasons, for example,  when the user scrolls basically we need to offet what is 
in the video buffer by one line an so on, upward of downward, then append the new line that is on the file buffer then to the video buffer. 

There is a scenario where the file is too large, so we need to read it by chunks, so when the user scrolls to a region out of the file buffer we need to read the next chunk of the file and append it to the file buffer, then we need to do the same for the video buffer.

-------------
| Disk File | : File stored in disk.
-------------
    |
-------------
| File Buffer | : Size of the max chunk of a file to be read
-------------
    |
----------------
| Video Buffer | : 80x25 memory space
----------------

We need to define the structs involved. I´ll avoid using linked lists as i want to keep it simple and fast.
