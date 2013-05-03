
//
// qchat -
// this is the RPC code executed only by the sequencer
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>

#include "qchat.h"

// For sendDatagram function:
#define BUFLEN 556
#define NPACK 10
#define PORT 9930
#define SRV_IP "127.0.0.1"

#define INITIAL_CLIENT_COUNT 8
#define MSG_BUF_SIZE 256

// If DEBUG is set, various debugging statements
// are triggered to help debug RPC calls mostly
// #define DEBUG

// Global pointer to clist, ptr needed bc of unknown size
const int LOCALPORT = 10001;
clist *clients;
msg_recv *msg_buffer;

int32_t initialized = FALSE;
int32_t alloc_clients_size;
uint32_t seq_num = 0;

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

	// Allocate 128 message buffer:
	// sizeof(msg_recv) is 18 BYTES
	msg_buffer = malloc(sizeof(msg_recv)*MSG_BUF_SIZE);

	initialized = TRUE;

	return 0;
}

void destroy_data_structures() {
  if(initialized) {
    if(clients != NULL) {
      if (clients->clientlist.clientlist_val != NULL) {
        free(clients->clientlist.clientlist_val);
      }
      free(clients);
    }
    // Add free to each msg_buffer element:
    if(msg_buffer != NULL) {
    	if (msg_buffer->msg_sent != NULL) {
    		free(msg_buffer->msg_sent);
    	}
    	if (msg_buffer->user_sent != NULL){
    		free(msg_buffer->user_sent);
    	}
    }
  }
}

// Errors, printing value of s: 
void diep(char *s) {
  perror(s);
  exit(1);
}

// Sends UDP packet:
void sendDatagram(msg_type_t msgType, uint32_t sequence, uname sd_user, msg_send sd_message) {

  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  
  char buf[BUFLEN]; 

  // Attempt to open a socket:
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
    diep("socket");
  }

  // memset - fill a byte string with a byte value:
  memset((char *) &si_other, 0, sizeof(si_other));

  // AF_INET is the address family for IP:
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);

  // int inet_aton(const char *cp, struct in_addr *inp);
  // Converts internet host addr cp from IPv4 numbers and dots notation
  // into binary form (in network byte order), stores it inside the
  // structure that *inp points to.
  if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  #ifdef DEBUG
  printf("Sending packet %d\n\n", seq_num);
  printf("sd_message: %s, sd_user: %s\n", sd_message, sd_user);
  #endif

  // int sprintf(char * restrict str, const char * restrict format, ...);
  sprintf(buf, "%d,%d,%s,%s", msgType, sequence, sd_user, sd_message);


  // ssize_t sendto(int socket, const void *buffer, size_t length, int flags, 
  // const struct sockaddr *dest_addr, socklen_t dest_len);
  if (sendto(s, buf,  BUFLEN, 0, &si_other, slen)==-1) {diep("sendto()");}

  close(s);
}


clist *join_1_svc(cname *userdata, struct svc_req *rqstp) {

	// Add to clientlist
	// return current clientlist
	// multicast new member msg, seq#

	// Validate username, if it exists error

	// If initialized is FALSE, run init procedure:
	if (!initialized) {
		init_data_structures();
	}
	
	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].userName = (uname) strdup(userdata->userName);
	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].hostname = (hoststr) strdup(userdata->hostname);
	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].lport = userdata->lport;
  	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].leader_flag = userdata->leader_flag;

  	clients->clientlist.clientlist_len++;

	return(clients);
}

int *send_1_svc(msg_recv *message, struct svc_req *rqstp) {

	static int result = 0;
	int i;

	if (!initialized) {
		init_data_structures();
	}

	// Move message into msg_buffer
	//printf("before: %d\n", seq_num);
	msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent = (char *) malloc(sizeof(msg_send)*strlen(message->msg_sent));
	msg_buffer[seq_num % MSG_BUF_SIZE].user_sent = (char *) malloc(sizeof(uname)*strlen(message->user_sent));
	strcpy(msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent, message->msg_sent);
	strcpy(msg_buffer[seq_num % MSG_BUF_SIZE].user_sent, message->user_sent);
	msg_buffer[seq_num % MSG_BUF_SIZE].seq_num = seq_num;
	msg_buffer[seq_num % MSG_BUF_SIZE].msg_type = message->msg_type;

	#ifdef DEBUG
	printf("msg_sent: %s", msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent);
	printf("user_sent: %s\n", msg_buffer[seq_num % MSG_BUF_SIZE].user_sent);
	printf("seq: %d\n", seq_num);
	printf("msg_type: %d\n", msg_buffer[seq_num % MSG_BUF_SIZE].msg_type);
	#endif

	// assign seq#
	// multicast to clients, on fail/retry:
	// 		remove client from clist
	//		multicast exist msg, seq# 

	//printf("msg_sent after debug, before sd shit %s", msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent);
	//msg_send sd_message = msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent;
	//uname sd_user = msg_buffer[seq_num % MSG_BUF_SIZE].user_sent;
	//printf("before sd message: %s\n", sd_message);
	//printf("before sd user: %s\n", sd_user);
	
	// type, sequence, sender, msg
	sendDatagram(
				msg_buffer[seq_num % MSG_BUF_SIZE].msg_type,
				msg_buffer[seq_num % MSG_BUF_SIZE].seq_num,
				msg_buffer[seq_num % MSG_BUF_SIZE].user_sent, 
				msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent);

	// Knock up seq_num by 1:
	seq_num = seq_num + 1;

	return(&result);

}

msg_recv *redeliver_1_svc(u_int * seq_num, struct svc_req *rqstp) {

  return NULL;
}

int *exit_1_svc(msg_recv *argp, struct svc_req *rqstp) {

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

uint32_t *heartbeat_1_svc(uint32_t *argp, struct svc_req *rqstp) {
	static uint32_t result = 0;
	if (!initialized) {
		init_data_structures();
	}
	result = *argp ++;
	return((&result));
}

void *shutdownserv_1_svc(void *rpc, struct svc_req *rqstp) {
  destroy_data_structures();
  exit(0);
  return NULL;
}

