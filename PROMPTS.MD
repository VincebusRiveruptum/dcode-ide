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
```
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
```
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
```
We need to define the structs involved. I´ll avoid using linked lists as i want to keep it simple and fast.

### Next step

Now i'm planning on the next step, we already have:

1. Keyboard Handling by ISR chaining allowing us to detect combined keystrokes and type text on screen with DOS codepage
and keyboard languaje management.

2. Video handling, basic TUI functions for filling, clearing the screen and drawing squares with a background/foreground color.

The next step could be implementing the TUI arquitecture which i already made a draft in @TUI.H , it is very HTML like as has some styling and event handling similutides. This will help programmers with a previous web development mindest. 

So i want to hear your opinion, what do you think about this? Lets discuss.

AI pointed me the following:

```
    "This is a very ambitious and well-structured draft! Mimicking the web (HTML/CSS-lite) for DOS is a brilliant way to make the framework accessible. It shifts the focus from "pixel-pushing" to "structural composition," which is exactly what a modern TUI should do.

    Here are my thoughts on your architecture and some technical considerations we should address:

    1. The Layout Engine (The "Flexbox" Challenge)
    You've included justifyContent, direction, and alignment. This implies a Box Model.

    Coordinate Calculation: We'll need a function that traverses the tree (e.g., Dialog -> Form -> FormGroup -> Inputs) to calculate absolute screen coordinates based on relative spacing.
    Constraint System: In the web, boxes can shrink/grow. In DOS, we are limited by char cells. We should decide if FormGroup has a fixed width or if it calculates it based on children.
    2. Event Dispatching & Focus
    Since you have ISR-based keyboard handling, we need a Focus Manager:

    How do we know which Input or Button is currently "active"?
    We'll need a "Global Focus Index" or a pointer to the active struct.
    For buttons, we need to map the keys[4] in your struct to the actual ISR keyboardTable to trigger the click callback.
    3. Memory Management (The char * vs char[] dilemma)
    DOS apps often struggle with heap fragmentation.

    Your structs use char *text. This is flexible but requires malloc/free.
    Suggestion: For labels or tooltips, we might want to support both static strings (string literals) and dynamic buffers for inputs.
    4. Component Hierarchy (Polymorphism in C)
    Right now, each component is a separate struct. To make a generic "Render System" or "Event Dispatcher," we might want a "Base" header for all UI elements.

    5. Z-Order & Transparency
    Since you have Dialog and Form, we'll likely have overlapping elements.

    Do we want the background ▒ to be preserved when a dialog closes?
    We might need a Screen Buffer (an array in memory) to "compose" the scene before sending it to 0xB8000."
```

### Common-Header Arquitecture 

After creating the arquitecture base, where i defined all the structs involved for the app's main requeriment, the AI suggested some small improvment by giving the Common-Header pattern arquitecture. 

This consists on creating a struct that will be an attribute in all structs, this struct will contain the common attributes of all structs, such as:

```c
typedef struct BaseElement {
    ElementType type;
    unsigned short id;
    struct Position position;
    char name[TUI_MAX_NAME_SIZE];
    bool enabled;
    bool visible;
} BaseElement;
```

This compared to the Composition pattern, this reduces memory fragmentation and improves perfomance. BaseElement will always be on offset 0 of the Element struct.

Now after defining the architecture, we have to start making instances of these elements and implement rendering.

---

# 22.01.2026

Today i did some progress, the rectangle drawing was pending and now at 12:08 PM i finished it, they work
great, not the prettiest code but it does the job.

Ok, i'm back but at 21:46 PM, i did the following:

- Box container with borders: It works fine, altrough not the cleanest code, fully done by myself
- Text container: This one the AI had to help me, barebone textbox where a text buffer is copied to the screen
at certain position and boundaries, such as a rectangle, where it crops if the text is out of bounds.
- Cursor movement with arrow keys: The cursor now can be controlled with the arrow keys, discussing with the AI
i learned that special keys such as arrows are sent in pairs, first the scancode 0xE0 and then the arrow key scancode, so i had to do two getch() calls to get the arrow key scancode.

The cursor movement works but the input does not match the cursor position, i mean, the cursor moves but if i want
to type something, the text shows up in the wrong place.

I got the idea of making a function for dumping the text buffer to a file. This maaybe could be equivalent
to a screen capture. Also this could be useful for dumping a text buffer instead of the screen buffer so i can
save an edited file.

Ok, i fixed the cursor position issue, i had to replace the getch() with my own function where just writes the
character directly in the memory at the cursor position, nothing else, super simple.

---

# 23.01.2026

I'm thinking of  handling scrolling, first i had the idea of having two buffers, because this will be an IDE or text editor, the text displayed in screen is a mix between the TUI and the real file content to be edit. So, one buffer will be used for displaying and the other for storing the text content, wich will be copied to the video buffer then (the first buffer mentioned). This would able for separaiton of concerns as the TUI will be not touched, and the scroll could be done inside the file buffer (second buffer) to the video buffer . 

However, i'm pretty sure that there are hardware approach for this, maybe with int 21 i could do this, i dont know,  what do you think, don't give me a full solution i just want to discuss this and see which approach we could take.

Actually there could be three buffers: 
* File buffer: This will hold the entire file content.
* Editor buffer: This will hold part of the file buffer that will be rendered on screen.
* Screen buffer: This will hold the entire screen content, including the TUI and the editor buffer.

This came along with the idea of scroll handling, my aproach could be a bit cpu demanding though. The idea is that when the user reaches top or bottom of the Screen buffer, the Editor buffer will scroll up
by copying all the contents again from the file buffer from Y + 1 to the text buffer. 

So, when the user reaches bottom, the start poistion from the FILE buffer will be Y++, and when the user reaches top, the start position from the FILE buffer will be Y--. From these positions the contents
will be copied to the editor buffer, and then to the screen buffer, and the scroll effect will be achieved.

### Undo/Redo

What happens when the user edits the text buffer? The FILE buffer will be updated

What happens when the user wants to undo or redo? I noticed in Borland C when the user undo, the cursor 
is also affected, this means the app register all activity. Should i do the same? 

What would be considered an "activity" or "history"? Storing the entire affected file buffer for each
activity could be a dumb idea as it could literally multiply the file size in memory per activity. A 
better approach could be storing a path of changes, with activity metadata, such as the cursor position, special key pressed, cursor movement, etc. 

---

# 25.01.2026

Ok, yesterday during the morning i had to do a huge refactoring of the code by making the arenas dynamic and took the decision of every file opened, is an arena of its own as files have their own lifecycle. This came from the problem that in a editor files could be opened and closed everytime, and with the current arena implementaiton, this could not have been posible as i didn't consider that i was allocating files in a single arena. 

Yesterday afternoon i wasn't home as i hanged out with a friend at his home, but today in the afternoon i came back and took back the pending work. I finished the arena refactor and also defined that there must be three buffers

- VIDEO BUFFER (textmemptr): All the visible stuff, the screen buffer.
- EDITOR BUFFER (editormemptr): Prior to rendering everything on screen with Video Buffer, all the TUI and file contents will be rendered here first, then copied to video buffer.
- FILE BUFFER (own arena): This is actually the file content whnere the user is editing, it will
be updated by the user actions.
- TEMP BUFFER (tempBuffer): A general purpose used for coping between buffers, used in DRAW.C/dw_writeBuffer()

I was kinda tired so the AI helped me in the most miserable and small bugs, at the end we got the buffering working. 

The next challenge is to format the buffered text because tabs, line breaks are visible as weird
characters.

# 29.01.2026

I'm back, it's 21:01 PM, trying to remember what is pending.

The file content display is wrong. I need to fix it.

So, the scheme would be the following


  FILE BUFFER    <---------- DIRECT TO HERE-
                                            |
      |                                     |
    FORMATTING TABS AND SPACES ...          |
      |                                     |    
      |                                     |
  EDITOR BUFFER                             |
                                            |
      |                                     |
      |                                     |
    DIRECT COPY                             |
      |                                     |
      |                                     |
      |                                     |
  VIDEO BUFFER                              |
      |                                     |
      |                                     |
      |                                     |   
      |                                     |
      v                                     |
    USER TYPING  ---------------------------

So the cursor pointing the position on screen it will be translated to the file buffer position and after typing the character will be formatted and so on.

---

# 30.01.2026

Yesterday i was so tired, today i do too. Yesteday atleast got the formatting right and some sort of scrolling, however my job now is to get it 
fixed.

The scroll kinda works but half-assed due that the scroll logic is not counting the 'artifical line jumps' where visually are line jumps but not in the file buffer. 

A solution could be instead of scrolling by changing the file offset (which is the start point of where the file buffer is being copied to the editor buffer), could be scrolling the editor buffer which already has the file content formatted, and this way we could consider the line jumps as real line jumps.

Ok. my approach was no good and not scalable, i had to talk with the AI and suggested handling the lines in a LIST, and  it was 
much more easier to handle.

Now i have to do the following:

- Open file with command line arguments OK
- Special keywords coloring OK --- WIP THO 
- Better keyboard handling OK
- Update lines on input
- Create/Remove lines when user does a line break or backspace
- Save file by reading all lines and parsing into the file buffer.
- Create file windows

- Close file
- Create new file

---
