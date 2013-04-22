

CFLAGS="-g -Wall -pedantic -O0 -std=gnu99"

# make client
cc $CFLAGS -c -o qchat.o qchat.c
cc $CFLAGS -c qchat_clnt.c
cc $CFLAGS -c qchat_xdr.c
cc -o qchat qchat.o qchat_clnt.o qchat_xdr.o

# make server
cc $CFLAGS -c -o qchat_server.o qchat_server.c
cc $CFLAGS -c qchat_svc.c
cc -o qchat_svc qchat_server.o qchat_svc.o qchat_xdr.o
