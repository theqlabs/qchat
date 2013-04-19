CC=gcc
CFLAGS= -c -g -Wall
LDFLAGS=
SOURCES=qchat.c qchat_clnt.c qchat_svc.c qchat_xdr.c qchat.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dchat

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf *o qchat
