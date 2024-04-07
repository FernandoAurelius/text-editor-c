#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Global variable storing the original flags of the terminal.
struct termios orig_termios;

/* Disable the modifications made by enableRawMode() returning the flags modifieds 
* to its original state using a variable "orig_termios".
*/
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/* Turn off echoing by getting the original terminal flags (tcgetattr), 
* read the current attributes into a struct, 
* modify the struct using bitwise operations to disable ECHO
* and finally applies the modified attributes back to the terminal
* using tcsetattr. At exit, disable all the changes made by calling disableRawMode().
*/
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    tcgetattr(STDIN_FILENO, &raw);
    
    // Disable CTRL-S, CTRL-Q, fix CTRL-M in terminal.
    raw.c_iflag &= ~(IXON | ICRNL);
    // Disable output process of newline (\r\n).
    raw.c_oflag &= ~(OPOST);
    // Disable ECHO, CTRL-C, CTRL-Z, CTRL-V and CTRL-O in terminal.
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;
    
    /* Reads 1 byte of the standard input, and put it into the variable c, until there's 
    * no more bytes to read. 
    *
    * Now, the program will 'quit' after the user enters the letter 'q'.
    */
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		// In case it's a non-printable character, just shows its ASCII code.
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			// In case it's a printable character, shows its ASCII code and its 
			// character representation.
			printf("%d ('%c')\r\n", c, c);
		}
    }
    return 0;
}