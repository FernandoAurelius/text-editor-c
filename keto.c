#include <termios.h>
#include <unistd.h>

/* Turn off echoing by getting the original terminal flags (tcgetattr), 
* read the current attributes into a struct, 
* modify the struct using bitwise operations to disable ECHO
* and finallyy applies the modified attributes back to the terminal
* using tcsetattr.
*/
void enableRawMode() {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);
    
    raw.c_lflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;
    
    /* Reads 1 byte of the standard input, and put it into the variable c, until there's 
    * no more bytes to read. 
    *
    * Now, the program will 'quit' afther the user enters the letter 'q'.
    */
    while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    return 0;
}