
//
// qchat -
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

//#define HELLO_PORT 9930
//#define HELLO_GROUP "225.0.0.37"
//#define SRV_IP "127.0.0.1"

#define INITIAL_CLIENT_COUNT 8
#define MSG_BUF_SIZE 256

// Global pointer to clist, ptr needed bc of unknown size
clist *clients;
msg_recv *msg_buffer;
char buf[BUFLEN];

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

	struct sockaddr_in addr;
	int fd, cnt, i;

	if ((fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0) {
		fprintf(stderr, "SOCKET CREATION FAILED");
		exit(1);
	}

	/* set up destination address */
	//memset(&addr,0,sizeof(addr));
	//addr.sin_family=AF_INET;
	//addr.sin_port=htons(HELLO_PORT);

	// iterate through client list setting address,port and sendto:
	for (i=0; i<clients->clientlist.clientlist_len; i++) {
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(clients->clientlist.clientlist_val[i].lport);
		if (inet_aton(clients->clientlist.clientlist_val[i].hostname, &addr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
		sprintf(buf, "%d,%d,%s,%s", msgType, sequence, sd_user, sd_message);

		if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "sendto");
			exit(1);
		}

	}
	//close(fd);
}

void mcClients(uname userName, hoststr hostname, int lport, int leader_flag) {

	//struct ip_mreq {
	//    struct in_addr imr_multiaddr; /* multicast group to join */
	//    struct in_addr imr_interface; /* interface to join on */
	//}

	struct sockaddr_in addr;
	int fd, cnt, i;

	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		fprintf(stderr, "SOCKET CREATION FAILED");
		exit(1);
	}

	// int sprintf(char * restrict str, const char * restrict format, ...);

	for (i=0; i<clients->clientlist.clientlist_len; i++) {
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(clients->clientlist.clientlist_val[i].lport);
		if (inet_aton(clients->clientlist.clientlist_val[i].hostname, &addr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
		
		sprintf(buf, "%d,%s,%s,%d,%d", NEWUSER, userName, hostname, lport, leader_flag);

		if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "sendto");
			exit(1);
		}
	}
	//close(fd);
}

void mcExit(uname *exitingUser) {

	struct sockaddr_in addr;
	int fd, cnt, i;

	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		fprintf(stderr, "SOCKET CREATION ERROR");
		exit(1);
	}

	// iterate through client list setting address,port and sendto:
	for (i=0; i<clients->clientlist.clientlist_len; i++) {
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(clients->clientlist.clientlist_val[i].lport);
		if (inet_aton(clients->clientlist.clientlist_val[i].hostname, &addr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
		sprintf(buf, "%d,%s", USEREXIT, exitingUser);

		if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			fprintf(stderr, "sendto");
			exit(1);
		}
	}
	//close(fd);
}

int *join_1_svc(cname *userdata, struct svc_req *rqstp) {

	int unameErr;
	status_code status;

	// Add to clientlist

	// Validate username, if it exists error

	// If initialized is FALSE, run init procedure:
	if (!initialized) {
		init_data_structures();
	}

	// if username returns with an error, set status code to UNAMEINUSE=2:
	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].userName = (uname) strdup(userdata->userName);

	if (clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].userName == NULL) {
		status = 2;
	}

	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].hostname = (hoststr) strdup(userdata->hostname);
	clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].lport = userdata->lport;
  clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].leader_flag = userdata->leader_flag;

	// if clients struct is empty, then JFAILURE, status_code = 1:
	if (clients == NULL) {
		status = 1;
	}

  // Multicast client list
  // set message type to NEWUSER=1
  mcClients(
  			clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].userName,
  			clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].hostname,
  			clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].lport,
  			clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].leader_flag);

  clients->clientlist.clientlist_len++;
  
	return((int *)&status);
}

int *send_1_svc(msg_recv *message, struct svc_req *rqstp) {

	static int result = 0;
	int i;

	if (!initialized) {
		init_data_structures();
	}
 	

	if (message->msg_sent == NULL || strlen(message->msg_sent) == 0 || message->msg_sent[0] == EOF) {
		return(NULL);
	}

	// Move message into msg_buffer
	//printf("before: %d\n", seq_num);
	msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent = (char *) malloc(sizeof(msg_send)*strlen(message->msg_sent));
	msg_buffer[seq_num % MSG_BUF_SIZE].user_sent = (char *) malloc(sizeof(uname)*strlen(message->user_sent));
	strcpy(msg_buffer[seq_num % MSG_BUF_SIZE].msg_sent, message->msg_sent);
	strcpy(msg_buffer[seq_num % MSG_BUF_SIZE].user_sent, message->user_sent);
	msg_buffer[seq_num % MSG_BUF_SIZE].seq_num = seq_num;
	msg_buffer[seq_num % MSG_BUF_SIZE].msg_type = message->msg_type;

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

	// requests a specific packet from msg_buffer and returns it
	//&(msg_buffer[seq_num % MSG_BUF_SIZE]);

	return &(msg_buffer[*seq_num % MSG_BUF_SIZE]);

}

int *exit_1_svc(uname *user, struct svc_req *rqstp) {

	static int result;
	int i;

	if (!initialized) {
		init_data_structures();
	}

	for (i=0; i<clients->clientlist.clientlist_len; i++) {
		if(strcmp(user, clients->clientlist.clientlist_val[i].userName) == 0) {
			if(i<(clients->clientlist.clientlist_len-1)) {
				memmove(&(clients->clientlist.clientlist_val[i]), 
								&(clients->clientlist.clientlist_val[i+1]), 
								sizeof(cname)*clients->clientlist.clientlist_len-1-i);
				break;
			}
		}

	}

	mcExit(user);

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
