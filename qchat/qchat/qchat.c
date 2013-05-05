//
//  qchat.c
//  qchat - a distributed chat program with the following features
//      - Fully-ordered Multicast UDP Protocol
//      - Sequencer-Client Model implementing an election protocol for failure recovery
//      - Automatic Recovery from UDP-based failures
//
//  @authors:
//  Andrew Righter - @theqlabs (GitHub/Twitter)
//  Michael Collis - mcollis@cis.upenn.edu

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdint.h>
#include "qchat.h"
#include "holdback_queue.h"

#define HOLD_Q_SIZE 128

#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
#define MSGBUFSIZE 256

// If DEBUG is set, various debugging statements
// are triggered to help debug RPC calls mostly
// #define DEBUG

// Function Declarations
void print_client_list(clist *);
void getLocalIp(char*);
void holdElection();

// Constants, Scope: Global
const int PORTSTRLEN = 6;
const int HEARTBEAT_DELAY = 3000;
int isSequencer = 0;
CLIENT *clnt;
clist *clientlist;
HoldbackQueue *queue;
char buf[BUFLEN];

int msgCompare(const void* m1, const void* m2) {
  if(((msg_recv*)m1)->seq_num > ((msg_recv*)m2)->seq_num) {
    return 1;
  } else if (((msg_recv*)m1)->seq_num < ((msg_recv*)m2)->seq_num) {
    return -1;
  } else {
    return 0;
  }
}

static void sig_handler(int signal) {
  if(signal == SIGTERM) {
    pthread_exit(NULL);
  }
}

// Errors, printing value of s:
void diep(char *s) {
  perror(s);
  exit(1);
}

msg_recv* parseMessage(char * buf) {
    msg_recv* inMsg = malloc(sizeof(msg_recv));
    if(inMsg == NULL) {
      diep("Incoming message allocation failed");
    }
    int token = strcspn(buf, ",");
    if(token > 1) {
      diep("Malformed incoming message at first token");
    }
    buf[token] = '\0';
    (*inMsg).msg_type = (unsigned int) strtoul(&(buf[0]), NULL, 10);
    int nextToken = strcspn(&(buf[++token]), ",");
    if(nextToken > 10) {
      diep("Malformed incoming message at second token");
    }
    buf[token + nextToken] = '\0';
    (*inMsg).seq_num = (unsigned int) strtoul(&(buf[token]), NULL, 10);
    token = token+nextToken + 1;
    nextToken = strcspn(&(buf[token]), ",");
    if(nextToken > 32) {
      diep("Malformed incoming message at third token");
    }
    buf[token + nextToken] = '\0';
    (*inMsg).user_sent = strdup(&(buf[token]));
    token = token+nextToken + 1;
    buf[BUFLEN-1] = '\0';
    (*inMsg).msg_sent = strdup(&(buf[token]));
    return inMsg;
}

// Receives UDP packet:
void recvDatagram(void) {

  if (signal(SIGTERM, sig_handler) == SIG_ERR) {
      fputs("Error occurred setting a SIGTERM handler.\n", stderr);
      pthread_exit(NULL);
   }

  int expectedSeq = -1;

  struct sockaddr_in addr;
  int fd, nbytes,addrlen;
  struct ip_mreq mreq;
  u_int yes=1;

  /* create what looks like an ordinary UDP socket */
  if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
    perror("socket");
    exit(1);
  }

  /* allow multiple sockets to use the same PORT number */
  if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
    perror("Reusing ADDR failed");
    exit(1);
   }

  /* set up destination address */
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
  addr.sin_port=htons(HELLO_PORT);

  /* bind to receive address */
  if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
    perror("bind");
    exit(1);
  }

  /* use setsockopt() to request that the kernel join a multicast group */
  mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
  mreq.imr_interface.s_addr=htonl(INADDR_ANY);

  if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
    perror("setsockopt");
    exit(1);
  }

     /* now just enter a read-print loop */
  while (1) {

      addrlen=sizeof(addr);

      if ((nbytes=recvfrom(fd,buf,MSGBUFSIZE,0,(struct sockaddr *) &addr,&addrlen)) < 0) {
         perror("recvfrom");
         exit(1);
      }

    msg_recv* inMsg = parseMessage(&buf);
    hq_push(queue, inMsg);
    msg_recv* nextMsg = hq_pop(queue);
    if(nextMsg == NULL) {
      printf("No messages in the message queue\n");
    }
    if(expectedSeq == -1) {
      expectedSeq = (*nextMsg).seq_num;
    } else if((*nextMsg).seq_num > expectedSeq) {
      int targetMsg = (*nextMsg).seq_num;
      expectedSeq++;
      while (expectedSeq < targetMsg) {
        nextMsg = redeliver_1(&expectedSeq, clnt);
        printf("%s: %s\n", (*nextMsg).user_sent, (*nextMsg).msg_sent);
        expectedSeq++;
      }
    }
    expectedSeq++;
    printf("%s: %s\n", (*nextMsg).user_sent, (*nextMsg).msg_sent);
    free(nextMsg);
  }
  //close(fd);
}

/*
void* electionHandler() {

  if (signal(SIGTERM, sig_handler) == SIG_ERR) {
      fputs("Error occurred setting a SIGTERM handler.\n", stderr);
      pthread_exit(NULL);
   }

  uint32_t hbIndex = 0;
  while (hbIndex >= 0) {
    uint32_t * result = heartbeat_1(&hbIndex, clnt);
    hbIndex ++;
    if(result == NULL) {
      // Lenin is dead. Call an election.
      holdElection();
    }
    printf("%d\n", hbIndex);
    sleep(HEARTBEAT_DELAY);
  }

  pthread_exit(NULL);
}
*/

int init_client(char* host) {

  // If clnt handle already exists, destroy:
  if (clnt != NULL) {
    clnt_destroy(clnt);
  }
  clnt = clnt_create((char*)host, QCHAT, QCHATVERS, "udp");

  // Check to see if the client handle was created
  if (clnt == NULL) {
    clnt_pcreateerror((char*)host);
    fprintf(stderr, "Could not create a client handle on %s\n", host);
    return 1;
  }
  return 0;
}

int main(int argc, char * argv[]) {

  //pid_t pID = fork();
  //if (pID == 0) {
  //  execlp("./qchat_svc", NULL, (char *) 0);
  //}

  // Join Variables:
  cname  userdata;

  // Send Variables:
  int *result_send;
  msg_send  arg_send;

  // Exit Variables:
  int *result_exit;
  msg_send  arg_exit;

  // Heartbeat Variables:
  int *result_heartbeat;
  int arg_heartbeat;

  queue = hq_init(msgCompare, HOLD_Q_SIZE);

  // Usage:
  if (argc > 3 || argc < 2) {
    printf("Usage ./dchat nickname [host server IP:PORT]\n");
    return 1;
  }

  // Obtains local IP address of the client
  char *localHostname = (char*) malloc((size_t)INET_ADDRSTRLEN);
  if (localHostname == NULL) {
    printf("Chat localHostname memory allocation failed. Exiting...\n");
    return 1;
  }
  getLocalIp(localHostname);
  if (strlen(localHostname) == 0) {
    printf("Could not obtain a local hostname");
    return 1;
  }
  //Proceed with chat joining or creation
  if (strlen(argv[1]) > MAX_USR_LEN-1) {
    //Truncate your foolishly long username
    argv[1][MAX_USR_LEN-1] = '\0';
  }

  //Create the RPC client objects
  if (argc == 3) {
    //Joining an existing chat
    char *remoteHostname = argv[2];
    printf("%s joining an existing chat on %s, listening on %s:%d\n", argv[1], remoteHostname, localHostname, HELLO_PORT);
    // create client handle, check health:
    int isClientAlive = init_client(remoteHostname);
    if (isClientAlive == 1) {
      clnt_pcreateerror(localHostname);
      printf("Unable to activate a new chat on %s, try again later.\n", localHostname);
      return 1;
    }
    // if connection doesn't succeed
    if (clnt == NULL) {
      clnt_pcreateerror(remoteHostname);
      printf("Sorry, no chat is active on %s, try again later.\nBye.\n", remoteHostname);
      return 1;
    }
  } else {
    //Creating a new chat
    printf("%s started a new chat, listening on %s:%d\n", argv[1], localHostname, HELLO_PORT);
    isSequencer = 1;
    int isClientAlive = init_client(localHostname);
    if (isClientAlive == 1) {
      clnt_pcreateerror(localHostname);
      printf("Unable to activate a new chat on %s, try again later.\n", localHostname);
      //return 1;
    }
  }

  userdata.userName = (uname) argv[1];
  userdata.hostname = (hoststr) localHostname;
  userdata.lport = HELLO_PORT;
  userdata.leader_flag = isSequencer;

  // Call to join_1:
  int* joinResult = join_1(&userdata, clnt);
  if (joinResult == NULL) {
    clnt_perror(clnt, "RPC request to join chat failed");
  } else if (joinResult == UNAMEINUSE) {
    printf("Sorry, there is another user in the chat with that username. Please try again.\n");
    return 1;
  } else if (joinResult == UNAMEINVALID) {
    printf("Sorry, username is not valid. Only alphanumeric characters are allowed in chat usernames.\nPlease try again.\n");
    return 1;
  } else if (joinResult == JFAILURE) {
    printf("Joining the specified chat failed. Please try again later.\n");
    return 1;
  }

  // Message handling thread
  pthread_t handlerThread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&handlerThread, &attr, recvDatagram, NULL);

  // Election handling thread
  //pthread_t electionThread;
  //pthread_create(&electionThread, &attr, electionHandler, NULL);

  // The code that mimics chat functionality by replaying inputmsg
  char* inputmsg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
  if (inputmsg == NULL) {
    return 1;
  }
  // Message struct msg_recv:
  msg_recv msg;

  while (read(0, inputmsg, MAX_MSG_LEN) > 0) {

      inputmsg[MAX_MSG_LEN-1]='\0';
      inputmsg[strlen(inputmsg)-1] = '\0';
      msg.msg_sent = (msg_send) strdup(inputmsg);
      msg.user_sent = userdata.userName;
      msg.msg_type = TEXT;

      // Calls the send_1 RPC:
      int* result_send = send_1(&msg, clnt);
      if (result_send == NULL) {
        clnt_perror(clnt, "RPC request to send message failed:");
      }
      if (msg.msg_sent != NULL) {
        free (msg.msg_sent);
      }
      if (inputmsg != NULL) {
        free(inputmsg);
      }
      inputmsg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
  }

  //Fix shutdown procedure when ctrl-c is pressed
  pthread_attr_destroy(&attr);
  pthread_kill(handlerThread, SIGTERM);
  //pthread_kill(electionThread, SIGTERM);

  exit_1(clnt);

  //Terminate RPC process
  if(clnt != NULL) {
    if(userdata.leader_flag == 1) {
      shutdownserv_1(NULL, clnt);
    }
    clnt_destroy(clnt);
  }

  // Cleaning up memory!
  if(localHostname != NULL) {
    free(localHostname);
  }
  return 0;
}

// Automatic local IP address discovery
void getLocalIp(char* buf) {
  buf[0] = '\0';
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    printf("Error discovering local IP address. Exiting...\n");
    return;
  }

  const char* openDnsAddr = "208.67.222.222";
  uint16_t dnsPort = 53;
  struct sockaddr_in* socketadd;
  socketadd = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
  socketadd->sin_family = AF_INET;
  socketadd->sin_addr.s_addr = inet_addr(openDnsAddr);
  socketadd->sin_port = htons(dnsPort);

  int err = connect(sock, (const struct sockaddr*) socketadd, sizeof(*socketadd));
  if (socketadd != NULL) {
    free(socketadd);
  }

  if (err < 0 || sock == -1) {
    printf("Error discovering local IP address. Exiting...\n");
    return;
  }
  struct sockaddr_in sockname;
  socklen_t socknamelen = sizeof(sockname);
  err = getsockname(sock, (struct sockaddr*) &sockname, &socknamelen);
  if (sock == -1) {
    printf("Error discovering local IP address. Exiting...\n");
    return;
  }

  const char* p = inet_ntop(AF_INET, &sockname.sin_addr, buf, INET_ADDRSTRLEN);
  if (p == NULL) {
    printf("Error discovering local IP address. Exiting...\n");
    return;
    }
  close(sock);
}

void print_client_list(clist * client_list) {

  int numClients = client_list->clientlist.clientlist_len, i;
  for (i=0 ; i < numClients; i++) {
    printf("%s %s:%d", client_list->clientlist.clientlist_val[i].userName,
                       client_list->clientlist.clientlist_val[i].hostname,
                       client_list->clientlist.clientlist_val[i].lport);

    if (client_list->clientlist.clientlist_val[i].leader_flag == 1) {
      printf(" (Leader)");
    }
     printf("\n");
   }
}

void holdElection() {
  //Elect a new sequencer
}

