CC=gcc
CFLAGS= -g -Wall -pedantic -O0 -std=gnu99
LDFLAGS= -pthread
SOURCES1=qchat.c qchat_clnt.c qchat_xdr.c qchat.h holdback_queue.c
OBJECTS1=$(SOURCES1:.c=.o)
SOURCES2=qchat_server.c qchat_svc.c qchat_xdr.c qchat.h
OBJECTS2=$(SOURCES2:.c=.o)
client=qchat
server=qchat_svc

all: $(SOURCES1) $(client)

$(client): $(OBJECTS1)
	$(CC) $(LDFLAGS) $(OBJECTS1) -o $@
$(server) : $(OBJECTS2)
	$(CC) $(LDFLAGS) $(OBJECTS2) -o $@


clean:
	rm -rf *o qchat
	rm qchat_svc
