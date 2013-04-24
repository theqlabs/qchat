/*
    An External Data Representation (XDR) file describing the protocol
    definition for the qChat program. ONC RPCGEN is being used as the
    protocol compiler for this project.

*/

const MAX_MSG_LEN = 512;        /* 64 Bytes */
const MAX_USR_LEN = 32;         /* 4 Bytes  */
const MAX_IP_LEN =  32;		    /* 4 Bytes  */

typedef string msg_send<MAX_MSG_LEN>;
typedef string uname<MAX_USR_LEN>;
typedef string ip_port<MAX_IP_LEN>;

struct cname {
    uname userName;
    ip_port hostname;
    int leader_flag;
};

typedef struct cname cname;

struct clist {
    cname clientlist<>;
};

struct msg_recv {
    msg_send msg_sent;
    uname user_sent;
    int seq_num;
};

program QCHAT {
    version QCHATVERS {
        clist JOIN(cname) = 1;
        int SEND(msg_send) = 2;
        int EXIT(msg_send) = 3;
        int HEARTBEAT(unsigned int) = 4;
    } = 1;
} = 0x20000001;