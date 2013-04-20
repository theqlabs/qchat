# make client
cc -g -c -o qchat.o qchat.c
cc -g -c qchat_clnt.c
cc -g -c qchat_xdr.c
cc -o qchat qchat.o qchat_clnt.o qchat_xdr.o

# make server
cc -g -c -o qchat_server.o qchat_server.c
cc -g -c qchat_svc.c
cc -o qchat_svc qchat_server.o qchat_svc.o qchat_xdr.o
