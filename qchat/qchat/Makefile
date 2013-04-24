CC=gcc
CFLAGS= -g -Wall -pedantic -O0 -std=gnu99
LDFLAGS= -pthread
SOURCES=qchat.c qchat_clnt.c qchat_xdr.c qchat.h holdback_queue.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=qchatgit

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf *o qchat
	rm qchat_svc
