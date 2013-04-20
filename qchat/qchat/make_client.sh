cc -c -o qchat.o qchat.c
cc -c qchat_clnt.c
cc -c qchat_xdr.c
cc -o qchat qchat.o qchat_clnt.o qchat_xdr.o
