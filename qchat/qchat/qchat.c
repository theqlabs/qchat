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

#include "qchat.h"


int main(int argc, char * argv[]) {

  CLIENT *clnt;
  void *result_1;
  char * printmessage_1_arg;
  char *localHostname = (char*) malloc((size_t)INET_ADDRSTRLEN);
  const int LOCALPORT = 10001;
  const int PORTSTRLEN = 6;
  int isSequencer = 0;
  int seqInitialized = FALSE;
  struct clientlist* clientList;

  if (localHostname == NULL) {
    printf("Chat localHostname memory allocation failed. Exiting...\n");
    return 1;
  }

  if (argc > 3 || argc < 2) {
    printf("Usage ./dchat nickname [host server IP:PORT]\n");
    return 1;
  }

  //Local IP address discovery protocol
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    printf("Error discovering local IP address. Exiting...\n");
    return 1;
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
    return 1;
  }
  struct sockaddr_in sockname;
  socklen_t socknamelen = sizeof(sockname);
  err = getsockname(sock, (struct sockaddr*) &sockname, &socknamelen);
  if (sock == -1) {
    printf("Error discovering local IP address. Exiting...\n");
    return 1;
  }

  const char* p = inet_ntop(AF_INET, &sockname.sin_addr, localHostname, INET_ADDRSTRLEN);
  if (p == NULL) {
    printf("Error discovering local IP address. Exiting...\n");
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
  //clientList =
  if (me == NULL) {
    printf("Error on client name memory allocation. Exiting...\n");
    return 1;
  }
  strncpy(me->userName, usrName, MAX_USR_LEN);

  char portString[PORTSTRLEN];
  sprintf(portString, "%d", LOCALPORT);
  char localIpPortStr[MAX_IP_LEN];
  strncpy(localIpPortStr, localHostname, strlen(localHostname)+1);
  localIpPortStr[strlen(localHostname)] = ':';
  localIpPortStr[strlen(localHostname)+1] = '\0';
  strncat(localIpPortStr, portString, strlen(portString));
  memcpy((void*)&(*me).hostname, localIpPortStr, (size_t)MAX_IP_LEN);

  if (argc == 3) {
    //Joining an existing chat
    remoteHostname = argv[2];
    printf("%s joined an existing chat on %s, listening on %s\n", usrName, remoteHostname, localHostname);
    // create a CLIENT handle
    clnt = clnt_create(localHostname, QCHAT, QCHATVERS, (char*)"udp");

    // if connection doesn't succeed
    if (clnt == NULL) {
      clnt_pcreateerror(localHostname);
      printf("Sorry, no chat is active on " << localHostname << ", try again later. " << endl;
      return 1;
    }

  } else {
    //Creating a new chat
    printf("%s started a new chat, listening on %s\n", usrName, localHostname);
    isSequencer = 1;

  }

  me->leader_flag = isSequencer;

  if(isSequencer == 1) {

  }

  //string myMessage = "";





  // // run PRINTMESSAGE function from qchat_server.cpp
  // result_1 = printmessage_1(&printmessage_1_arg, clnt);

  // printf("Welcome " << usrName << " to qchat on " << localHostname << endl;
  // printf(usrName << ": ";
  // cin >> myMessage;

  if(localHostname != NULL) {
    free(localHostname);
  }
  return 0;

}

int * join_1_svc (cname * client, struct svc_req* req) {

}

int * send_1_svc(msg_send * msg, struct svc_req* req) {

}

int * deliver_1_svc(msg_recv * msg, struct svc_req* req) {

}


