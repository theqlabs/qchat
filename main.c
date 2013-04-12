//
// qChat - a fully-ordered distributed chat program, using a multicast protocol via UDP
//      @author - Andrew Righter
//      @author - Michael Collis

// TODO
//  build message input system; msgMaxLen=512 chars usrnameMaxLen=32 chars.;                // enforce during UDP packet wrapper, otherwise buffer overflow could occur with data types
//  build input (stdin) output (stdout) message system locally;
//  ensure each function is separated;
//  convert simple I/O system into RPC system;
//  write option parser [0,1,2 arguments]
// ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#include <stdio.h>
#include <stdlib.h>

// function prototypes are needed to "predeclare" readLine() otherwise GCC will yell
char* readLine(); 

// argv is argument vector, contains a series of char pointers to these arguments 
//  (first element is always name of the program itself)
// argc is argument count, contains number of arguments passed to the program from 
//  command line PLUS ONE
int main(int argc, char *argv[]) {                               
        if (argc != 2) {
                printf("Usage: %s nickname\n", argv[0]);
                return (EXIT_FAILURE);
        }
        printf("%s says: %s \n", argv[1], readLine()); 
        // return 0 / EXIT_SUCCESS (defined in stdlib.h) or return 1 / EXIT_FAILURE;
        return (EXIT_SUCCESS);                                        
}

char* readLine() {
        char *line = NULL;
        // unsigned type, non-negative value (8 BYTES)
        size_t len = 0;                               
        // signed type (8 BYTES), return type needed by getline()
        ssize_t read;                                            
        // getline reads a line from stream (stdin), stores addr of buffer into *line
        read = getline(&line, &len, stdin);                      
        return line;
        // free the memory allocation for line pointer
        free(line);    
}

