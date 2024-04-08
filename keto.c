#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Global variable storing the original flags of the terminal.
struct termios orig_termios;

void die(const char *s) {
    perror(s);
    exit(1);
}

/* Disable the modifications made by enableRawMode() returning the flags modifieds 
* to its original state using a variable "orig_termios".
*/
void disableRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

/* Turn off echoing by getting the original terminal flags (tcgetattr), 
* read the current attributes into a struct, 
* modify the struct using bitwise operations to disable ECHO
* and finally applies the modified attributes back to the terminal
* using tcsetattr. At exit, disable all the changes made by calling disableRawMode().
*/
void enableRawMode() {
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcsetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;

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

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main() {
    enableRawMode();
    
    /* Reads 1 byte of the standard input, and put it into the variable c, until there's 
    * no more bytes to read. 
    *
    * Now, the program will 'quit' after the user enters the letter 'q'.
    */
    while(1) {
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if(iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if(c == 'q') break;
    }
    return 0;
}