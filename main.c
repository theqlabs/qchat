//
// qChat - a fully-ordered distributed chat program, using a multicast protocol via UDP
//      @author - Andrew Righter
//      @author - Michael Collis

// TODO
//  build dynamically allocated message input system;
//  build input (stdin) output (stdout) message system locally;
//  ensure each function is separated;
//  convert simple I/O system into RPC system;
//
// ssize_t getline(char **lineptr, size_t *n, FILE *stream);


#include <stdio.h>
#include <stdlib.h>

void readLine();                                // accepts user message, buffer size dynamically allocated
void printLine();                               // prints message into stdout

void readLine() {
        char *line = NULL;
        size_t len = 0;                         // unsigned type, non-negative value (8 BYTES)
        ssize_t read;                           // signed type (8 BYTES)

        read = getline(&line, &len, stdin);     // getline reads a line from stream (stdin), stores addr of buffer into *line
        free(line);                             // free allocation of memory the *line was using to store message
}

void printLine() {
        // get line from readLine() and print it out
        printf("andrew says: %s\n", line);
}

int main() {
    readLine();
}
