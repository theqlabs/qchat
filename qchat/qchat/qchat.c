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
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "qchat.h"

void print_client_list(clientlist *);
void getLocalIp(char*);

const int LOCALPORT = 10001;
const int PORTSTRLEN = 6;
clientlist* clist;

void* messageHandler(void* inputclist) {
  printf("Thread created successfully\n");
  clientlist* clist = (clientlist*) inputclist;
  if(inputclist == NULL) {
    printf("Socket listener received an empty clientlist. Exiting...\n");
    return (void*)1;
  }

  printf("Succeeded, current users:\n");
  //print_client_list(clist);
  int sockid;
  if((sockid = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
    perror("Error creating listening UDP socket");
    return (void *)1;
  }

  struct sockaddr_in* socketadd = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
  socketadd->sin_family = AF_INET;
  socketadd->sin_port = htons(LOCALPORT);

  if((bind(sockid, (struct sockaddr*)socketadd, sizeof(*socketadd)))<0) {
    close(sockid);
    perror("Error binding to listening UDP socket");
    return (void *)1;
  }
  printf("Server socket ok\n");
  close(sockid);
  return (void *)1;

}

int main(int argc, char * argv[]) {
  int isSequencer = 0;
  CLIENT *clnt;
  char *localHostname = (char*) malloc((size_t)INET_ADDRSTRLEN);


  if (localHostname == NULL) {
    printf("Chat localHostname memory allocation failed. Exiting...\n");
    return 1;
  }

  if (argc > 3 || argc < 2) {
    printf("Usage ./dchat nickname [host server IP:PORT]\n");
    return 1;
  }

  getLocalIp(localHostname);
  if (strlen(localHostname) == 0) {
    return 1;
  }

  //Proceed with chat joining or creation
  if (strlen(argv[1]) > MAX_USR_LEN-1) {
    //Truncate your foolishly long username
    argv[1][MAX_USR_LEN-1] = '\0';
  }
  uname usrName = (uname) argv[1];
  char* remoteHostname;

  cname* me = (cname *) malloc(sizeof(cname));
  //clist =
  if (me == NULL) {
    printf("Error on client name memory allocation. Exiting...\n");
    return 1;
  }

  memcpy(&(me->userName), usrName, strlen(usrName));

  char portString[PORTSTRLEN];
  sprintf(portString, "%d", LOCALPORT);
  char localIpPortStr[MAX_IP_LEN];
  strncpy(localIpPortStr, localHostname, strlen(localHostname)+1);
  localIpPortStr[strlen(localHostname)] = ':';
  localIpPortStr[strlen(localHostname)+1] = '\0';
  strncat(localIpPortStr, portString, strlen(portString));
  memcpy(&(me->hostname), localIpPortStr, MAX_IP_LEN);

  //Create the RPC client objects
  if (argc == 3) {
    //Joining an existing chat
    remoteHostname = argv[2];
    printf("%s joining an existing chat on %s, listening on %s:%d\n", usrName, remoteHostname, localHostname, LOCALPORT);
    // create a CLIENT handle
    clnt = clnt_create(remoteHostname, QCHAT, QCHATVERS, "udp");

    // if connection doesn't succeed
    if (clnt == NULL) {
      clnt_pcreateerror(remoteHostname);
      printf("Sorry, no chat is active on %s, try again later.\nBye.\n", remoteHostname);
      return 1;
    }

  } else {
    //Creating a new chat
    printf("%s started a new chat, listening on %s:%d\n", usrName, localHostname, LOCALPORT);
    isSequencer = 1;
    clnt = clnt_create(localHostname, QCHAT, QCHATVERS, "udp");
    if (clnt == NULL) {
      clnt_pcreateerror(localHostname);
      printf("Unable to activate a new chat on %s, try again later.\n", localHostname);
      //return 1;
    }
  }

  me->leader_flag = isSequencer;
  clientlist *clist;
  //clientlist *clist = join_1(&me, clnt);

  pthread_t handlerThread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&handlerThread, &attr, messageHandler, (void*)clist);
  //
  char inputmsg[MAX_MSG_LEN];
  while (inputmsg[0]!= EOF) {
    if (fgets(inputmsg, sizeof inputmsg, stdin)) {
      inputmsg[MAX_MSG_LEN-1]='\0';
      inputmsg[strlen(inputmsg)-1] = '\0';
      puts(inputmsg);
      //int* result = send_1(&inputmsg);
    }
  }

  pthread_attr_destroy(&attr);

  if(me!= NULL) {
    free(me);
  }

  if(localHostname != NULL) {
    free(localHostname);
  }
  return 0;

}

void getLocalIp(char* buf) {
  //Local IP address discovery protocol
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

  if (sock == -1) {
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
}

void print_client_list(clientlist * clist) {
  int numClients = sizeof(*clist)/sizeof(cname), i;
  for (i=0 ; i < numClients; i++)
  {
    //printf("%s %s", ((cname)clist[i]).userName, ((cname)clist[i]).hostname);
    //if (((cname)clist[i]).leader_flag = 1) {
    //  printf("(Leader)");
   // }
    printf("\n");
  }
}
