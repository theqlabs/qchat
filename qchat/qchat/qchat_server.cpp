//
//  qchat_server.cpp
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
#include <rpc/rpc.h>
#include "qchat.h"

using namespace std;

// Function Declaration
void printMessage(string);

// Global Variable
string clientMessage = "";

void printMessage(string clientMessage) {
    
    cout << "HELLO BITCHES!" << endl;
}