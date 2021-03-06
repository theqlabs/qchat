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
#define BUFLEN 556

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


typedef char *hoststr;
#ifdef __cplusplus
extern "C" bool_t xdr_hoststr(XDR *, hoststr*);
#elif __STDC__
extern  bool_t xdr_hoststr(XDR *, hoststr*);
#else /* Old Style C */
bool_t xdr_hoststr();
#endif /* Old Style C */


enum msg_type_t {
	TEXT = 0,
	NEWUSER = 1,
	USEREXIT = 2,
	ELECTION = 3,
};
typedef enum msg_type_t msg_type_t;
#ifdef __cplusplus
extern "C" bool_t xdr_msg_type_t(XDR *, msg_type_t*);
#elif __STDC__
extern  bool_t xdr_msg_type_t(XDR *, msg_type_t*);
#else /* Old Style C */
bool_t xdr_msg_type_t();
#endif /* Old Style C */


enum status_code {
	JSUCCESS = 0,
	JFAILURE = 1,
	UNAMEINUSE = 2,
	UNAMEINVALID = 3,
};
typedef enum status_code status_code;
#ifdef __cplusplus
extern "C" bool_t xdr_status_code(XDR *, status_code*);
#elif __STDC__
extern  bool_t xdr_status_code(XDR *, status_code*);
#else /* Old Style C */
bool_t xdr_status_code();
#endif /* Old Style C */


struct cname {
	uname userName;
	hoststr hostname;
	int lport;
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
	u_int seq_num;
	msg_type_t msg_type;
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
extern "C" int * join_1(cname *, CLIENT *);
extern "C" int * join_1_svc(cname *, struct svc_req *);
#define SEND ((rpc_uint)2)
extern "C" int * send_1(msg_recv *, CLIENT *);
extern "C" int * send_1_svc(msg_recv *, struct svc_req *);
#define EXIT ((rpc_uint)3)
extern "C" int * exit_1(uname *, CLIENT *);
extern "C" int * exit_1_svc(uname *, struct svc_req *);
#define REDELIVER ((rpc_uint)4)
extern "C" msg_recv * redeliver_1(u_int *, CLIENT *);
extern "C" msg_recv * redeliver_1_svc(u_int *, struct svc_req *);
#define HEARTBEAT ((rpc_uint)5)
extern "C" u_int * heartbeat_1(u_int *, CLIENT *);
extern "C" u_int * heartbeat_1_svc(u_int *, struct svc_req *);
#define SHUTDOWNSERV ((rpc_uint)6)
extern "C" void * shutdownserv_1(void *, CLIENT *);
extern "C" void * shutdownserv_1_svc(void *, struct svc_req *);

#elif __STDC__
#define JOIN ((rpc_uint)1)
extern  int * join_1(cname *, CLIENT *);
extern  int * join_1_svc(cname *, struct svc_req *);
#define SEND ((rpc_uint)2)
extern  int * send_1(msg_recv *, CLIENT *);
extern  int * send_1_svc(msg_recv *, struct svc_req *);
#define EXIT ((rpc_uint)3)
extern  int * exit_1(uname *, CLIENT *);
extern  int * exit_1_svc(uname *, struct svc_req *);
#define REDELIVER ((rpc_uint)4)
extern  msg_recv * redeliver_1(u_int *, CLIENT *);
extern  msg_recv * redeliver_1_svc(u_int *, struct svc_req *);
#define HEARTBEAT ((rpc_uint)5)
extern  u_int * heartbeat_1(u_int *, CLIENT *);
extern  u_int * heartbeat_1_svc(u_int *, struct svc_req *);
#define SHUTDOWNSERV ((rpc_uint)6)
extern  void * shutdownserv_1(void *, CLIENT *);
extern  void * shutdownserv_1_svc(void *, struct svc_req *);

#else /* Old Style C */
#define JOIN ((rpc_uint)1)
extern  int * join_1();
extern  int * join_1_svc();
#define SEND ((rpc_uint)2)
extern  int * send_1();
extern  int * send_1_svc();
#define EXIT ((rpc_uint)3)
extern  int * exit_1();
extern  int * exit_1_svc();
#define REDELIVER ((rpc_uint)4)
extern  msg_recv * redeliver_1();
extern  msg_recv * redeliver_1_svc();
#define HEARTBEAT ((rpc_uint)5)
extern  u_int * heartbeat_1();
extern  u_int * heartbeat_1_svc();
#define SHUTDOWNSERV ((rpc_uint)6)
extern  void * shutdownserv_1();
extern  void * shutdownserv_1_svc();
#endif /* Old Style C */

#endif /* !_QCHAT_H_RPCGEN */
