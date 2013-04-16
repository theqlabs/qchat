//
//  main.cpp
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

using namespace std;

int main(int argc, const char * argv[]) {

    string usrName = "";
    
    cout << "Please enter your username: ";
    cin >> usrName;
    cout << "Hello " << usrName << endl;
    
    return 0;

}

