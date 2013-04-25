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

// For recvDatagram function:
#define BUFLEN 512
#define NPACK 10
#define PORT 9930

// Function Declarations
void print_client_list(clist *);
void getLocalIp(char*);
void holdElection();

// Constants, Scope: Global
const int LOCALPORT = 10001;
const int PORTSTRLEN = 6;
const int HEARTBEAT_DELAY = 3000;
int isSequencer = 0;
CLIENT *clnt;
clist *clientlist;
HoldbackQueue *queue;

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

// Receives UDP packet: 
void recvDatagram(void) {

  // Create two structs of type sockaddr_in:
  struct sockaddr_in si_me, si_other;
  int s, i, slen = sizeof(si_other);
  char buf[BUFLEN];

  // Attempts to open a socket s, with parameters:
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
    diep("socket");
  }

  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;

  /// htons - converts unsigned short integer hostshort from host byte order to network byte order
  si_me.sin_port = htons(PORT);

  // htonl - converts unsigned integer netlong from network byte order to host byte order
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // Attempts to bind to socket s
  // what is si_me ??
  if (bind(s, &si_me, sizeof(si_me))==-1) {
    diep("bind");
  }

  for (i=0; i<NPACK; i++) {
    if (recvfrom(s, buf, BUFLEN, 0, &si_other, &slen)==-1) {
      diep("recvfrom()");
    }
    printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
  }

  close(s);
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

  pid_t pID = fork();
  if (pID == 0) {
    execlp("./qchat_svc", NULL, (char *) 0);
  }

  // Join Variables:
  clist  *result_join;
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
    printf("%s joining an existing chat on %s, listening on %s:%d\n", argv[1], remoteHostname, localHostname, LOCALPORT);
    // create client handle, check health:
    int isClientAlive = init_client(localHostname);
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
    printf("%s started a new chat, listening on %s:%d\n", argv[1], localHostname, LOCALPORT);
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
  userdata.lport = LOCALPORT;
  userdata.leader_flag = isSequencer;

  // Call to join_1:
  result_join = join_1(&userdata, clnt);
  if (result_join == NULL) {
    clnt_perror(clnt, "RPC request to join chat failed");
  }


  // Message handling thread
  pthread_t handlerThread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  // What's the purpose of result_join here? if recvDatagram is the start routine:
  // If this causes problems, try NULL for the final parameter
  pthread_create(&handlerThread, &attr, recvDatagram, (void*)result_join);

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
      inputmsg[strlen(inputmsg)] = '\0';

      msg.msg_sent = (msg_send) strdup(inputmsg);
      msg.user_sent = userdata.userName;
      msg.msg_type = TEXT;

      // Calls the send_1 RPC:
      int* result_send = send_1(&msg, clnt);
      if (result_send == NULL) {
        clnt_perror(clnt, "RPC request to join chat failed:");
      }
      if (msg.msg_sent != NULL) {
        free (msg.msg_sent);
      }
      if (inputmsg != NULL) {
        free(inputmsg);
      }
      inputmsg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
  }

  pthread_attr_destroy(&attr);
  pthread_kill(handlerThread, SIGTERM);
  //pthread_kill(electionThread, SIGTERM);

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

