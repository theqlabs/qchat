//
//  qchat_client.cpp
//  qchat - a distributed chat program with the following features
//      - Fully-ordered Multicast UDP Protocol
//      - Sequencer-Client Model implementing an election protocol for failure recovery
//      - Automatic Recovery from UDP-based failures
//
//  @authors:
//  Andrew Righter - @theqlabs (GitHub/Twitter)
//  Michael Collis - mcollis@cis.upenn.edu 
//

#include <iostream>
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "qchat.h"

using namespace std;

int main(int argc, char * argv[]) {
    
  CLIENT *clnt;
  void *result_1;
  char * printmessage_1_arg;
  char *hostName = (char*) malloc(INET_ADDRSTRLEN);

  if (hostName == nullptr) {
    cout << "Chat hostname memory allocation failed" << endl;
    return 1;
  }
  
  string usrName = "";
  
  if (argc > 3 || argc < 2) {
    cout << "Usage ./dchat nickname [host server IP:PORT]" << endl;
    return 1;
  } else if (argc == 3) {
    // Joining an existing chat
    hostName = argv[2];
    cout << "" << endl;
  } else {
    // Creating a new chat
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
      cout << "Error discovering local IP address" << endl;
      return 1;
    }
    
    const char* openDnsAddr = "208.67.222.222";
    uint16_t dnsPort = 53;
    struct sockaddr_in socketadd;
    memset(&socketadd, 0, sizeof(socketadd));
    socketadd.sin_family = AF_INET;
    socketadd.sin_addr.s_addr = inet_addr(openDnsAddr);
    socketadd.sin_port = htons(dnsPort);
    
    int err = connect(sock, (const sockaddr*) &socketadd, sizeof(socketadd));
    if (sock == -1) {
      cout << "Error discovering local IP address" << endl;
      return 1;
    }
    sockaddr_in sockname;
    socklen_t socknamelen = sizeof(sockname);
    err = getsockname(sock, (sockaddr*) &sockname, &socknamelen);
    if (sock == -1) {
      cout << "Error discovering local IP address" << endl;
      return 1;
    }
    
    const char* p = inet_ntop(AF_INET, &sockname.sin_addr, hostName, INET_ADDRSTRLEN);
    if (p == NULL) {
      cout << "Error discovering local IP address" << endl;
      return 1;
    }
  }
  
  string myMessage = "";
        
  // create a CLIENT handle
  clnt = clnt_create(hostName, QCHAT, QCHATVERS, (char*)"udp");
    
  // if connection doesn't succeed
  if (clnt == NULL) {
      clnt_pcreateerror(hostName);
    cout << "Sorry, no chat is active on " << hostName << ", try again later. " << endl;
      return 1;
  }
  
  
  
  // run PRINTMESSAGE function from qchat_server.cpp
  result_1 = printmessage_1(&printmessage_1_arg, clnt);
    
  cout << "Welcome " << usrName << " to qchat on " << hostName << endl;
  cout << usrName << ": ";
  cin >> myMessage;
  
  if(hostName != nullptr) {
    free(hostName);
  }
  return 0;
    
}