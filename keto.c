/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

// Define function CTRL_KEY() using macro
#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

// Global struct storing the configs of our text editor.
struct editorConfig {
    // Variables storing the number of rows and columns of the size of the terminal.
    int screenrows;
    int screencols;
    // Global variable storing the original flags of the terminal.
    struct termios orig_termios;
};

struct editorConfig E;

/*** terminal ***/

// Shows the error message using perror and then exits the program, returning 1 as usual.
// Clear the screen before showing the error message.
void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

/* Disable the modifications made by enableRawMode() returning the flags modifieds 
* to its original state using a variable "orig_termios".
*/
void disableRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) die("tcsetattr");
}

/* Turn off echoing by getting the original terminal flags (tcgetattr), 
* read the current attributes into a struct, 
* modify the struct using bitwise operations to disable ECHO
* and finally applies the modified attributes back to the terminal
* using tcsetattr. At exit, disable all the changes made by calling disableRawMode().
*/
void enableRawMode() {
    // If tcgetattr crashes, shows a error message using the function die().
    if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcsetattr");
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;

    tcgetattr(STDIN_FILENO, &raw);
    
    // Disable CTRL-S, CTRL-Q, fix CTRL-M and disable miscellaneous flags in terminal.
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    // Disable output process of newline (\r\n).
    raw.c_oflag &= ~(OPOST);
    // Enable CS8 bit mask to set caracther size to 8 bits per byte.
    raw.c_lflag |= (CS8);
    // Disable ECHO, CTRL-C, CTRL-Z, CTRL-V and CTRL-O in terminal.
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    // Sets a timer for the read() function.
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // Same as the beginning.
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

// Reads the keypress from the user.
char editorReadKey() {
    int nread;
    char c;
    while((nread == read(STDIN_FILENO, &c, 1)) != 1) {
        // Error handling.
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

/*** output ***/

// Draws a tilde on the beginning of every row.
// After drawing, returns the cursor to the beginning of the line (left side of screen).
void editorDrawRows() {
    int y;
    for(y = 0; y < E.screenrows; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

// Clear the screen using escape sequences.
// Also returns the cursor to the left side of the screen.
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** input ***/

// Verifies if the keypress from the user is the CTRL-Q command. If it is, breaks the code.
void editorProcessKeyPress() {
    char c = editorReadKey();

    switch(c) {
        case(CTRL_KEY('q')):
            // Clears the screen before breaking.
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

/*** init ***/

// Initializes both screen rows and columns of our struct E.
void initEditor() {
    if(getWindowSize(&E.screenrows, &E.screencols) == -1 ) die("getWindowSize");
}

int main() {
    enableRawMode();
    initEditor();
    
    // Now we only call our function editorProcessKeyPress() on main.
    while(1) {
        editorRefreshScreen();
        editorProcessKeyPress();
    }
    return 0;
}