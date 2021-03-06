/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "qchat.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

int *
join_1(argp, clnt)
	cname *argp;
	CLIENT *clnt;
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, JOIN, xdr_cname, argp, xdr_int, &clnt_res, TIMEOUT) != RPC_SUCCESS)
		return (NULL);
	return (&clnt_res);
}

int *
send_1(argp, clnt)
	msg_recv *argp;
	CLIENT *clnt;
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, SEND, xdr_msg_recv, argp, xdr_int, &clnt_res, TIMEOUT) != RPC_SUCCESS)
		return (NULL);
	return (&clnt_res);
}

int *
exit_1(argp, clnt)
	uname *argp;
	CLIENT *clnt;
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, EXIT, xdr_uname, argp, xdr_int, &clnt_res, TIMEOUT) != RPC_SUCCESS)
		return (NULL);
	return (&clnt_res);
}

msg_recv *
redeliver_1(argp, clnt)
	u_int *argp;
	CLIENT *clnt;
{
	static msg_recv clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, REDELIVER, xdr_u_int, argp, xdr_msg_recv, &clnt_res, TIMEOUT) != RPC_SUCCESS)
		return (NULL);
	return (&clnt_res);
}

u_int *
heartbeat_1(argp, clnt)
	u_int *argp;
	CLIENT *clnt;
{
	static u_int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, HEARTBEAT, xdr_u_int, argp, xdr_u_int, &clnt_res, TIMEOUT) != RPC_SUCCESS)
		return (NULL);
	return (&clnt_res);
}

void *
shutdownserv_1(argp, clnt)
	void *argp;
	CLIENT *clnt;
{
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, SHUTDOWNSERV, xdr_void, argp, xdr_void, &clnt_res, TIMEOUT) != RPC_SUCCESS)
		return (NULL);
	return ((void *)&clnt_res);
}
