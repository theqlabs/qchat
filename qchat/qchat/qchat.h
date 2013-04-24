/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _QCHAT_H_RPCGEN
#define _QCHAT_H_RPCGEN

#define RPCGEN_VERSION	199506

#include <rpc/rpc.h>

#define MAX_MSG_LEN 512
#define MAX_USR_LEN 32
#define MAX_IP_LEN 32

typedef char *msg_send;
#ifdef __cplusplus
extern "C" bool_t xdr_msg_send(XDR *, msg_send*);
#elif __STDC__
extern  bool_t xdr_msg_send(XDR *, msg_send*);
#else /* Old Style C */
bool_t xdr_msg_send();
#endif /* Old Style C */


typedef char *uname;
#ifdef __cplusplus
extern "C" bool_t xdr_uname(XDR *, uname*);
#elif __STDC__
extern  bool_t xdr_uname(XDR *, uname*);
#else /* Old Style C */
bool_t xdr_uname();
#endif /* Old Style C */


typedef char *ip_port;
#ifdef __cplusplus
extern "C" bool_t xdr_ip_port(XDR *, ip_port*);
#elif __STDC__
extern  bool_t xdr_ip_port(XDR *, ip_port*);
#else /* Old Style C */
bool_t xdr_ip_port();
#endif /* Old Style C */


struct cname {
	uname userName;
	ip_port hostname;
	int leader_flag;
};
typedef struct cname cname;
#ifdef __cplusplus
extern "C" bool_t xdr_cname(XDR *, cname*);
#elif __STDC__
extern  bool_t xdr_cname(XDR *, cname*);
#else /* Old Style C */
bool_t xdr_cname();
#endif /* Old Style C */


#ifdef __cplusplus
extern "C" bool_t xdr_cname(XDR *, cname*);
#elif __STDC__
extern  bool_t xdr_cname(XDR *, cname*);
#else /* Old Style C */
bool_t xdr_cname();
#endif /* Old Style C */


struct clist {
	struct {
		u_int clientlist_len;
		cname *clientlist_val;
	} clientlist;
};
typedef struct clist clist;
#ifdef __cplusplus
extern "C" bool_t xdr_clist(XDR *, clist*);
#elif __STDC__
extern  bool_t xdr_clist(XDR *, clist*);
#else /* Old Style C */
bool_t xdr_clist();
#endif /* Old Style C */


struct msg_recv {
	msg_send msg_sent;
	uname user_sent;
	int seq_num;
};
typedef struct msg_recv msg_recv;
#ifdef __cplusplus
extern "C" bool_t xdr_msg_recv(XDR *, msg_recv*);
#elif __STDC__
extern  bool_t xdr_msg_recv(XDR *, msg_recv*);
#else /* Old Style C */
bool_t xdr_msg_recv();
#endif /* Old Style C */


#define QCHAT ((rpc_uint)0x20000001)
#define QCHATVERS ((rpc_uint)1)

#ifdef __cplusplus
#define JOIN ((rpc_uint)1)
extern "C" clist * join_1(cname *, CLIENT *);
extern "C" clist * join_1_svc(cname *, struct svc_req *);
#define SEND ((rpc_uint)2)
extern "C" int * send_1(msg_send *, CLIENT *);
extern "C" int * send_1_svc(msg_send *, struct svc_req *);
#define EXIT ((rpc_uint)3)
extern "C" int * exit_1(msg_send *, CLIENT *);
extern "C" int * exit_1_svc(msg_send *, struct svc_req *);
#define HEARTBEAT ((rpc_uint)4)
extern "C" int * heartbeat_1(int *, CLIENT *);
extern "C" int * heartbeat_1_svc(int *, struct svc_req *);

#elif __STDC__
#define JOIN ((rpc_uint)1)
extern  clist * join_1(cname *, CLIENT *);
extern  clist * join_1_svc(cname *, struct svc_req *);
#define SEND ((rpc_uint)2)
extern  int * send_1(msg_send *, CLIENT *);
extern  int * send_1_svc(msg_send *, struct svc_req *);
#define EXIT ((rpc_uint)3)
extern  int * exit_1(msg_send *, CLIENT *);
extern  int * exit_1_svc(msg_send *, struct svc_req *);
#define HEARTBEAT ((rpc_uint)4)
extern  int * heartbeat_1(int *, CLIENT *);
extern  int * heartbeat_1_svc(int *, struct svc_req *);

#else /* Old Style C */
#define JOIN ((rpc_uint)1)
extern  clist * join_1();
extern  clist * join_1_svc();
#define SEND ((rpc_uint)2)
extern  int * send_1();
extern  int * send_1_svc();
#define EXIT ((rpc_uint)3)
extern  int * exit_1();
extern  int * exit_1_svc();
#define HEARTBEAT ((rpc_uint)4)
extern  int * heartbeat_1();
extern  int * heartbeat_1_svc();
#endif /* Old Style C */

#endif /* !_QCHAT_H_RPCGEN */
