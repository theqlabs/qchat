//
// qchat -
// this is the RPC code executed only by the sequencer
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "qchat.h"

static clientlist *clist;
int initialized = FALSE;

int init_data_structures() {

	// init data structure function
	clist = malloc(sizeof(clist));
	// init data structure fields

	//(*clist).clientlist_len = 0;		// Difference between this
	clist->clientlist_len = 0;			// ... and this?

	initialized = TRUE;

	return 0;

}

clientlist *join_1_svc(cname *test, struct svc_req *rqstp) {

	// clientlist is an array of cname structs
	// struct cname {
	// 		uname userName;
	//		ip_port hostname;
	//		int leader_flag;
	// };

	// takes in struct CNAME (me) from client
	// returns clientlist when done

	// Add to clientlist
	// return current clientlist
	// multicast new member msg, seq#

	if (!initialized) {
		init_data_structures();
	}

	test->userName = (uname) "biatch";
	clist[clist->clientlist_len++].clientlist_val = test;

	return(clist);
}

int *send_1_svc(msg_send *argp, struct svc_req *rqstp) {

	static int result;

	// takes in string msg_send from client
	// returns int (ACK) when done

	// assign seq#
	// multicast to clients, on fail/retry:
	// 		remove client from clist
	//		multicast exist msg, seq#



	return(&result);
}

int *exit_1_svc(msg_send *argp, struct svc_req *rqstp) {

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

int *
heartbeat_1_svc(argp, rqstp)
	int64_t *argp;
	struct svc_req *rqstp;
{
	static int64_t result = 0;
	result = *argp ++;
	return((&result));
}

