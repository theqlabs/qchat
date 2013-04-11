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

char* readLine() {
        char *line = NULL;
        size_t len = 0;                         // unsigned type, non-negative value (8 BYTES)
        ssize_t read;                           // signed type (8 BYTES), return type needed by getline()

        read = getline(&line, &len, stdin);     // getline reads a line from stream (stdin), stores addr of buffer into *line
        return line;

        free(line);                             // free the memory allocation for line pointer
}

int main() {
        printf("andrew: %s\n", readLine());
}

