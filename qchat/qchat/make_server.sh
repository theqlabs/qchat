cc -c -o qchat_server.o qchat_server.c
cc -c qchat_svc.c
cc -o qchat_svc qchat_server.o qchat_svc.o qchat_xdr.o
