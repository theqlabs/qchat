//
// qchat -
// this is the RPC code executed only by the sequencer
// 

#include "qchat.h"

//clientlist *join_1(argp, rqstp)
clientlist *join_1(cname *argp, struct svc_req *rqstp) {	
	//cname *argp;
	//struct svc_req *rqstp; {

	static clientlist result;

	// takes in struct CNAME from client
	// returns clientlist when done

	// Add to clientlist
	// return current clientlist
	// multicast new member msg, seq#



	return(&result);
}

int *
send_1(argp, rqstp)
	msg_send *argp;
	struct svc_req *rqstp; {

	static int result;

	// takes in string msg_send from client
	// returns int (ACK) when done

	// assign seq#
	// multicast to clients, on fail/retry:
	// 		remove client from clist
	//		multicast exist msg, seq# 



	return(&result);
}

int *
exit_1(argp, rqstp)
	msg_send *argp;
	struct svc_req *rqstp; {

	static int result;

	// takes in string msg_send from client
	// returns int (ACK) when done

	// assign seq#
	// remove from clist, isLeaderCheck:
	// 		pickNewLeader
	//		multicast new leader
	// multicast exist msg, seq#



	return(&result);
}

