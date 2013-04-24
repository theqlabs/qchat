
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

void destroy_data_structures() {
  if(initialized) {
    if(clients != NULL) {
      if (clients->clientlist.clientlist_val != NULL) {
        free(clients->clientlist.clientlist_val);
      }
      free(clients);
    }
  }
}

void multicast_message(msg_recv* message) {
  
  if (!initialized) {
		init_data_structures();
	}

  struct addrinfo myhints;
struct addrinfo *myservinfo;
memset(&myhints, 0, sizeof myhints);
myhints.ai_family = AF_INET;
myhints.ai_socktype = SOCK_DGRAM;
myhints.ai_flags = AI_PASSIVE;
getaddrinfo(NULL, "12001", &myhints, &myservinfo);


  int i;
  for(i = 0; i < clients->clientlist.clientlist_len; i++) {
    const char* userAddr = clients->clientlist.clientlist_val[i].hostname;
    uint16_t userPort = (uint16_t) clients->clientlist.clientlist_val[i].lport;
    
    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    char* portStr = malloc(sizeof(char) * 7);
    sprintf(portStr, "%d", userPort);
    getaddrinfo(userAddr, portStr, &hints, &servinfo);    

    int sock = socket(myhints.ai_family, myhints.ai_socktype, 0);
    connect(sock, servinfo->ai_addr, servinfo->ai_protocol);
    if (sock == -1) {
      continue;
    }

struct sockaddr_in* socketadd;    
    socketadd = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
    memset(socketadd,0,sizeof(sizeof(struct sockaddr_in)));    
    socketadd->sin_family = AF_INET;
    socketadd->sin_addr.s_addr = inet_addr(userAddr);
    socketadd->sin_port = htons(userPort);

    if (sendto(sock,(void*)message->msg_sent,(size_t) strlen((char*)message->msg_sent),0,(struct sockaddr *) socketadd,sizeof(struct sockaddr)) < 0) {
	       perror("sendto");
	       continue;
	  }

    if (sendto(sock,(void*)message->user_sent, (size_t) strlen((char*)message->user_sent),0,(struct sockaddr *) socketadd,sizeof(struct sockaddr)) < 0) {
	       perror("sendto");
	       continue;
	  }

    if (sendto(sock,(void*)message->seq_num,sizeof(uint32_t),0,(struct sockaddr *) socketadd,sizeof(struct sockaddr)) < 0) {
	       perror("sendto");
	       continue;
	  }

    if (sendto(sock,(void*)message->msg_type,sizeof(msg_type_t),0,(struct sockaddr *) socketadd,sizeof(struct sockaddr)) < 0) {
	       perror("sendto");
	       continue;
	  }
/*
    write(sock, (void*)message->msg_sent , (size_t) strlen((char*)message->msg_sent));
    write(sock, (void*)message->user_sent , (size_t) strlen((char*)message->user_sent));
    write(sock, (void*)message->seq_num , sizeof(int));
    write(sock, (void*)message->msg_type, sizeof(msg_type_t));
*/
    close(sock);
  }
freeaddrinfo(myservinfo);
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
  clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].userName = (uname) strdup(userdata->userName);
  clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].hostname = (hoststr) strdup(userdata->hostname);
	//memcpy(&(clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].userName), &(userdata->userName), strlen((char*)userdata->userName));
//memcpy(&(clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].hostname), &(userdata->hostname), strlen((char*)userdata->hostname));
  clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].lport = userdata->lport;
  clients->clientlist.clientlist_val[clients->clientlist.clientlist_len].leader_flag = userdata->leader_flag;
  //memcpy(&(clients->clientlist.clientlist_val[clients->clientlist.clientlist_len]), userdata, sizeof(cname));
  
	clients->clientlist.clientlist_len++;

	// multicast new member to each client
	// multicast-nastiness: 
	/*
	 struct sockaddr_in addr;
     int fd, cnt;
     struct ip_mreq mreq;
     char *message="Hello, World!";

     // create what looks like an ordinary UDP socket
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }

     // set up destination address
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
     addr.sin_port=htons(HELLO_PORT);
     
     // now just sendto() our destination!
     while (1) {
	  if (sendto(fd,message,sizeof(message),0,(struct sockaddr *) &addr,
		     sizeof(addr)) < 0) {
	       perror("sendto");
	       exit(1);
	  }
	  sleep(1);
     }
     */

    // This is a temporary fix.
    //initialized = FALSE;

	return(clients);
}

int *send_1_svc(msg_recv *argp, struct svc_req *rqstp) {

	// takes in string msg_send from client
	// returns int (ACK) when done

	// assign seq#
	// multicast to clients, on fail/retry:
	// 		remove client from clist
	//		multicast exist msg, seq# 
	static int result = 0;

	if (!initialized) {
		init_data_structures();
	}

  multicast_message(argp);
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

