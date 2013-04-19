/* 

    An External Data Representation (XDR) file describing the protocol
    definition for the qChat program. ONC RPCGEN is being used as the
    protocol compiler for this project.

*/ 

const MAX_MSG_LEN = 512;        /* 64 Bytes */
const MAX_USR_LEN = 32;         /* 4 Bytes */

typedef string msg_send<MAX_MSG_LEN>;
typedef string uname<MAX_USR_LEN>;

struct cname {
    string uname<MAX_USR_LEN>;
    string ip_port<>;
    bool leader_flag<>;
};

struct msg_recv {
    msg_send msg_sent<>;
    uname user_sent<>;
    int seq_num;
};

program QCHAT {
    version QCHATVERS {
        int JOIN(cname) = 1;
        int SEND(msg_send) = 2;
        int DELIVER(msg_recv) = 3;
        int LISTNAMES(cname) = 4;
        msg_recv REQ_MSG(seq_num) = 5;
    } = 1;
} = 0x20000001;
