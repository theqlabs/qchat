//
//  qchat_client.cpp
//  qchat - a distributed chat program with the following features
//      - Fully-ordered Multicast UDP Protocol
//      - Sequencer-Client Model implementing an election protocol for failure recovery
//      - Automatic Recovery from UDP-based failures
//
//  @authors:
//  Andrew Righter - @theqlabs (GitHub/Twitter)
//  Michael Collis
//

#include <iostream>
#include "qchat.h"
#include <rpc/rpc.h>

using namespace std;

// removed const from arg vector to satisfy throwing argv[2] into char *host;
int main(int argc, char * argv[]) {
    
    CLIENT *clnt;
    void *result_1;
    char * printmessage_1_arg;
    char *host;

    string hostName = "";
    string usrName = "";
    string myMessage = "";
    
    if (argc != 3) {
        cout << "Usage ./qchat nickname server" << endl;
        return 1;
    }
    
    host = argv[2];
        
    // create a CLIENT handle
    clnt = clnt_create(host, QCHAT, QCHATVERS, "udp");
    
    // if connection doesn't succeed
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        return 1;
    }
    
    // run PRINTMESSAGE function from qchat_server.cpp
    result_1 = printmessage_1(&printmessage_1_arg, clnt);
    
    cout << "Welcome " << usrName << " to qchat on " << hostName << endl;
    cout << usrName << ": ";
    cin >> myMessage;
        
    return 0;
    
}