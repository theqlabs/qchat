
//
// qchat -
// this is the RPC code executed only by the sequencer
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "qchat.h"

#define INITIAL_CLIENT_COUNT 8

// Global pointer to clist, ptr needed bc of unknown size
clist *clients;
int32_t initialized = FALSE;
int32_t alloc_clients_size;

int init_data_structures() {

	// Allocate Memory for clist
	// Set fields to initial values
	clients = malloc(sizeof(clist));
	clients->clientlist.clientlist_len = 0;
	alloc_clients_size = INITIAL_CLIENT_COUNT;
	cname *list_values = (cname*)calloc((size_t)INITIAL_CLIENT_COUNT, sizeof(cname));
	clients->clientlist.clientlist_val = list_values;

	// If any of them are NULL error, exit
	if (clients == NULL || clients->clientlist.clientlist_val == NULL) {
		fprintf(stderr, "Error initializing data structures\n");
		return -1;
	}
	initialized = TRUE;

	return 0;
}

clist *join_1_svc(cname *userdata, struct svc_req *rqstp) {

	// takes in struct CNAME (me) from client
	// returns clientlist when done

	// Add to clientlist
	// return current clientlist
	// multicast new member msg, seq#

	// If initialized is FALSE, run init procedure:
	if (!initialized) {
		init_data_structures();
	}
	
	// Copy userdata into clist (using mem addrs.):
	memcpy(&(clients->clientlist.clientlist_val[clients->clientlist.clientlist_len]), userdata, sizeof(cname));
	clients->clientlist.clientlist_len++;

	return(clients);
}

int *send_1_svc(msg_send *argp, struct svc_req *rqstp) {

	// takes in string msg_send from client
	// returns int (ACK) when done

	// assign seq#
	// multicast to clients, on fail/retry:
	// 		remove client from clist
	//		multicast exist msg, seq# 
	static int result;

	if (!initialized) {
		init_data_structures();
	}

	return(&result);
}

int *exit_1_svc(msg_send *argp, struct svc_req *rqstp) {

	// takes in string msg_send from client
	// returns int (ACK) when done

	// assign seq#
	// remove from clist, isLeaderCheck:
	// 		pickNewLeader
	//		multicast new leader
	// multicast exist msg, seq#
	static int result;

	if (!initialized) {
		init_data_structures();
	}

	return(&result);
}

int *heartbeat_1_svc(uint32_t *argp, struct svc_req *rqstp) {

	static uint32_t result = 0;

	if (!initialized) {
		init_data_structures();
	}
	result = *argp ++;

	return((&result));
}

