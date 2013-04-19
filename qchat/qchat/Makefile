CC=gcc
CFLAGS= -g -Wall -pedantic -O0 -std=gnu99
LDFLAGS=
SOURCES=qchat.c qchat_clnt.c qchat_svc.c qchat_xdr.c qchat.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dchat

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf *o qchat
