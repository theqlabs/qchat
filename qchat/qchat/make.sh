# make client
cc -c -o qchat.o qchat.c
cc -c qchat_clnt.c
cc -c qchat_xdr.c
cc -o qchat qchat.o qchat_clnt.o qchat_xdr.o

# make server
cc -c -o qchat_server.o qchat_server.c
cc -c qchat_svc.c
cc -o qchat_svc qchat_server.o qchat_svc.o qchat_xdr.o
