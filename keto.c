#include <unistd.h>

int main() {
    char c;
    
    /* Reads 1 byte of the standard input, and put it into the variable c, until there's 
    no more bytes to read.*/
    while(read(STDIN_FILENO, &c, 1) == 1)
    return 0;
}