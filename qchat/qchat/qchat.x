/* 

    An External Data Representation (XDR) file describing the protocol
    definition for the qChat program. ONC RPCGEN is being used as the
    protocol compiler for this project.

*/ 

program QCHAT {
    version QCHATVERS {
        void PRINTMESSAGE(string) = 1; 
    } = 1;
} = 0x20000001;
