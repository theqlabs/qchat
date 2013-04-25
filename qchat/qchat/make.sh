
# -ggdb3 includes all info into symbol tables
# -O0 shuts off all optimizations for debugging results

CFLAGS="-w -ggdb3 -Wall -pedantic -O0 -std=gnu99"
LDFLAGS="-pthread"

# make client
cc $CFLAGS -c -o qchat.o qchat.c
cc $CFLAGS -c qchat_clnt.c
cc $CFLAGS -c qchat_xdr.c
cc $CFLAGS -c holdback_queue.c
cc -o qchat qchat.o qchat_clnt.o qchat_xdr.o holdback_queue.o

# make server
cc $CFLAGS -c -o qchat_server.o qchat_server.c
cc $CFLAGS -c qchat_svc.c
cc -o qchat_svc qchat_server.o qchat_svc.o qchat_xdr.o
