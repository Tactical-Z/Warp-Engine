#include "NetworkManager.h"
#include "server.h"
#include "Logger.h"

int InitServer(struct ConnectionData* _conData){

    // 1. Create the socket
    if(!CreateSocket(_conData))
        return 0;

    // 2. Assign IP & Port
    AssignIpPortLong(_conData, AF_INET, INADDR_ANY, PORT);
    
    // 3. Bind newly created socket to given IP and verificiation. 
    BindPortToAddress(_conData);
    
    // 4. Server is ready to listen and verification
    // listen() marks socket as passive, meaning it will be used to accept incomming conenction requests.
    // the arg 5 marks the backlog, which defines the max number of pending connections.

    return 1;
}

