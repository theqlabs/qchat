cc -g -c -o qchat.o qchat.c
cc -g -c qchat_clnt.c
cc -g -c qchat_xdr.c
cc -o qchat qchat.o qchat_clnt.o qchat_xdr.o
