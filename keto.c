#include <unistd.h>
#include <stdio.h>

int main() {
    char c;
    
    /* Reads 1 byte of the standard input, and put it into the variable c, until there's 
    * no more bytes to read. 
    *
    * Now, the program will 'quit' afther the user enters the letter 'q'.
    */
    while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    return 0;
}