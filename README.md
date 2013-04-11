qchat
=====

A distributed chat program written in C. 

qChat implements the following architecture: 
    Description: 
        qChat is a fully-ordered, multicast protocol UDP chat program. It is based on a leader-elected sequencer
        model and is designed to be distributed in nature. It will automatically recover from UDP-based failures (dropped
        packets, duplicated packets, ordering of packets.)

        A user either creates a new chat group or enters an existing one. If the user starts a new group, that user is then
        marked as the leader/sequencer and handles all leader functions. If the leader leaves the chat at any time, an
        election is called among current active users to elect the next leader/sequencer. This is done (quite primatively
        by analyzing the timestamp of when active users entered the group chat and choosing the longest chatting member.)

    Leader/Sequencer:
        The leader is responsible for ordering incoming messages from each client and sending them out via multicast
        UDP delivery to all members of the chat group. It is also the responsibility of the leader to detect and eliminate
        inactive/crashed users in the chat group. Any user who doesn't receive a response from the leader in a given amount
        of time can execute a leader election to choose a new leader/sequencer.

    Joining a Chat:
        A new client should be able to join a chat through any member, even if it's not the leader. If a client attempts
        to connect through a member, member responds back with leader IP:PORT information.
           
Design Details

    Abstract: So here is the way I imagine this working.

    Every client has access to the same functions, same queues, same messages. The only thing that
    dictates whether or not a client is currently a leader or a member is through the isLeader() boolean
    function which returns TRUE if client is the leader, otherwise FALSE. Having a single binary for everyone, this has to occur, also it
    makes it easier for correcting errors and leader re-election.

    A member executes ./qChat
        0 argument - prints HELP dialog, suggesting member enters a NICKNAME
        1 argument - uses arg1 as the name (EX: ./qChat q starts a new chat with user q)
        2 arguments - check arg1 is a name, arg2 should be IP:PORT to connect to existing chat

        ./qChat NICKNAME                    // STARTS CHAT ON DEFAULT NETWORK INTERFACE
        ./qChat NICKNAME IP_ADDRESS:PORT    // STARTS CHAT ON SPECIFIED IP:PORT

    IF user creates new chat:
        begin listening on network interface
        sets leaderCheck();
        execute addUser(); passing leader flag/arg

    IF user enters existing chat:
        attempts connection to IP:PORT
        sets leaderCheck();
        begins listening on network interface
        execute addUser();
        execute listUsers();

    Member Functions:
        
        sendMsg() -             sends a message to current leader's queue for sorting and sending to group
        printMsg() -            listens on network IP:PORT for messages from leader
        leaderCheck() -         checks who is the current leader
        addUser() -             adds self into the user list, maintained by each client
        listUsers() -           lists current members in the chat, asterisk next to the current leader (includes IP:PORT of each member)

        msgBufRaw() -           raw messages received by members
        msgBufSorted() -        timestamped, ordered messages ready to be sent to group


    Message Formats
        Types of messages:
            message                                 // part of the chat dialog
                 struct unordered                   // raw message from client
                    unsigned long int timestamp;    // timestamp of local machine of when msg was sent by user
                    char *msg;                      // var. length message
                    int mode;                       // [0] - Chat Mode
                                                    // [1] - System Message
                                                    // [2] - Leader Election Message
    
                struct ordered                      // message processed by sequencer and ready to send to group
                    unsigned long int timestamp;    
                    char *msg;
                    int seq_num;                    // UID assigned for sequencing messages into correct order


                struct userList {
                    unsigned long int timestamp;    // timestamp of users arrival into chat (FOR LEADER ELECTION)
                    char *usrName;                  // username (NICK) of the user
                    char *ipAddr;                   // the IP ADDRESS of the user
                    char *port;                     // source port of the user
                    bool isLeader;                  // set as TRUE if user is the leader, otherwise FALSE


    Sending
        Data Objects are converted into Bytes wrapped in UDP and sent out
    Receiving
        Bytes are converted back into Data Objects

#TODO:
# DO WE NEED TO SEND LENGTHS FOR VARIABLE-LENGTH DATA? (message, username)
# IS THE LEADER DOING ANY CONCURRENCY?

    NOTES: So we have unordered and ordered message formats, defined inside their own structs. Each has their own data
    fields. Unordered messages are created by a client and sent to the leader, processed and either sent to queue (msg)
    or sent directly to users (system events.) 

    Messages are crafted locally by each client and sent via UDP to everyone else in the group through a multicast protocol.
    Messages are received by the leader/sequencer, who tags them with a seq_num, sorts them into an ordered buffer
    and begins sending them back out to everyone through the printMsg(). 
    
    Asynchronous UDP Communication
        qChat will rely on asynch. UDP communication.
            SENDS REQUEST (RPC)
            WAITS FOR ACCEPTANCE
            ACCEPTS REQUEST

        Multicast Groups will be used for the chat, group is created for a new chat group and users are subscribed when
        logging in for the first time. This will allow for a group list of users who receive messages from this chat
        group, and it will allow multiple chat groups to exist! 

    Error control scheme for UDP
        Every user is going to keep a message queue{TIMESTAMP, NICKNAME, MSG} and each client will wait for an acknowledgement
        from the leader that message has been received. If an acknowledgement is not received by the leader/sequencer, the
        process is halted, message queue continues storing messages of MODE[0], and if the leader/sequencer is responsive,
        then the leader/sequencer does a sorting check on the message queue. Since each user is retaining a queue of the
        messages (WHO SENT IT and WHAT TIME it was sent) we can verify completeness of the chat. 

    Leader Election
        On an event (usually, a message not been acknowledged by the leader) a user can call an election to determine if the
        leader is still active and processing messages, and if not who is going to take his place. When an election is called
        a message (MODE[2]) is used to send communication to all users in the chat group. The list of current active users
        will be used to elect a new leader. The first name of the list will compare his timestamp against the next user in the
        list, whichever has the EARLIER timestamp wins that round and proceeds to the next user, this happens until there is
        only one user left, who is then elected the new leader/sequencer.






