/*

    An External Data Representation (XDR) file describing the protocol
    definition for the qChat program. ONC RPCGEN is being used as the
    protocol compiler for this project.

*/

const MAX_MSG_LEN = 512;        /* 64 Bytes */
const MAX_USR_LEN = 32;         /* 4 Bytes  */
const MAX_IP_LEN =  32;		/* 4 Bytes  */

typedef string msg_send<MAX_MSG_LEN>;
typedef string uname<MAX_USR_LEN>;
typedef string ip_port<MAX_IP_LEN>;
typedef cname clientlist<>;

struct cname {
    uname userName;
    ip_port hostname;
    int leader_flag;
};

struct msg_recv {
    msg_send msg_sent;
    uname user_sent;
    int seq_num;
};

program QCHAT {
    version QCHATVERS {
        int JOIN(cname) = 1;
        int SEND(msg_send) = 2;
        int DELIVER(msg_recv) = 3;
        int LISTNAMES(clientlist) = 4;
        msg_recv REQ_MSG(int) = 5;
    } = 1;
} = 0x20000001;
