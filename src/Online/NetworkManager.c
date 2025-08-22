
#include "NetworkManager.h"
#include "ThreadManager.h"
#include "server.h"
#include "client.h"

enum ConnectionSide connectionSide = CSI_NON;
enum ConnectionState connectionState = CST_DISCONECTED;

int InitNetwork(int* _argc, char *argv[], struct ConnectionData* _conData){

    if(*_argc >= 2){     
        WSADATA wsaData; // winwdows verriable for socket data
        int err = WSAStartup(MAKEWORD(2,2), &wsaData); // Inits winsock lib, MAKEWORD(2,2) specifies version IE 2.2, &wsaData is filled by function
        if(err != 0){
            SetErrnoFromWinsock();
            LOG_ERROR("WSAStartup Failed");
            return 0;
        }

        // Initalize server / client connection
        if(strcmp(argv[1], "-S") == 0){
            LOG_INFO("Initializing Server");
            if(InitServer(_conData)){
                connectionSide = CSI_SERVER;
            }
        } 
        else if(strcmp(argv[1], "-C") == 0){
            LOG_INFO("Initializing Client");
            if(InitClient(_conData)){
                connectionSide = CSI_CLIENT;
            }
        }
   } else {
        connectionSide = CSI_NON;   
   }

   return 1;
}

int CreateSocket(struct ConnectionData* _conData){ 
    
    // Socket() creates a socket, AF_INET spescifies the address family meaning IPv4,
    // SOCK_STREAM specifies socket type meaning TCP socket. 0 is the protocol meaning system chooses.
    _conData->sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if(_conData->sockfd == -1){
        SetErrnoFromWinsock();
        LOG_ERROR("Socket Creation Failed");
    } else {
        LOG_INFO("Socket successfully created");
        
        u_long mode = 1;
        SOCKET socketDescriptor = _conData->sockfd;
        if(ioctlsocket(socketDescriptor, FIONBIO, &mode) != 0){
            SetErrnoFromWinsock();
            LOG_ERROR("Failed so set non blocking mode");
            return 0;
        }
        return 1;
    }
    return 0;
};

void AssignIpPortLong(struct ConnectionData* _conData, int _socketType, u_long _networkInterfaces, int _portId){
    
    // memset() clears the mermoy of all serv_addr fields and sets all bytes to 0.
    _conData->addr_len = sizeof(struct sockaddr_in);
    memset(&_conData->addr, 0, _conData->addr_len);
    _conData->addr.sin_family = _socketType; // Define socket as IPV4
    _conData->addr.sin_addr.s_addr = htonl(_networkInterfaces); // INADDER_ANY tells the socket to bind to all availible network interfaces, 
                                                   // htonl() stands for Host TO Network Long and converts the host byte order to network byte order.
    _conData->addr.sin_port = htons(_portId); // Sets the port number, htons() stands for Host TO Network Short and converts port number to network byte order.
};

void AssignIpPortChar(struct ConnectionData* _conData, int _socketType, const char* _networkInterfaces, int _portId){
    
    // memset() clears the mermoy of all serv_addr fields and sets all bytes to 0.
    _conData->addr_len = sizeof(struct sockaddr_in);
    memset(&_conData->addr, 0, _conData->addr_len);
     _conData->addr.sin_family = _socketType; // Define socket as IPV4
     _conData->addr.sin_addr.s_addr = inet_addr(_networkInterfaces); // INADDER_ANY tells the socket to bind to all availible network interfaces, 
                                                   // htonl() stands for Host TO Network Long and converts the host byte order to network byte order.
     _conData->addr.sin_port = htons(_portId); // Sets the port number, htons() stands for Host TO Network Short and converts port number to network byte order.
};

int BindPortToAddress(struct ConnectionData* _conData){
     // bind() connects the socket to the port and address of the serv_addr struct
    if((bind(_conData->sockfd, (SA*)&_conData->addr, sizeof(_conData->addr))) != 0) {
        SetErrnoFromWinsock();
        LOG_ERROR("Socket bind failed");
    } else {
        LOG_INFO("Socket successfully binded");
        return 1;
    }
    return 0;
};

void* ServerListen(void* _arg){

    struct ConnectionData* _conData = (struct ConnectionData*)_arg;

    if((listen(_conData->sockfd, 5)) != 0){
        LOG_ERROR("Listen faild");
    } else {
       LOG_INFO("Initiate Listeing");
    }

    int frames = 0;
    while (threadStatus != TS_SHUTTINGDOWN) {
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(_conData->sockfd, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 2; // Wait up to 2 second
        timeout.tv_usec = 0;

        int result = select(_conData->sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (result > 0 && FD_ISSET(_conData->sockfd, &readfds)) {
            if (AcceptConection(_conData)) {
                ChatServer(_conData);   
                break;
            }
        } else if (result == 0){
            LOG_INFO("Sever still listining...");
        }
    }
    LOG_INFO("Server - Thread Closed");
}

void* ClientConnect(void* _arg){

    struct ConnectionData* _conData = (struct ConnectionData*)_arg;
    int retryCount = 0;

    while (threadStatus != TS_SHUTTINGDOWN)
    {
        switch (connectionState)
        {
        case CST_DISCONECTED:
            if(ConnectToSocket(_conData)){
                connectionState = CST_CONNECTING;
                LOG_WARN("TRIED TO CONNECT");
            }
            break;
        case CST_CONNECTING:
            switch (CheckSocketWriteable(_conData))
            {
            case 0:
                LOG_INFO("Client still looking for connection...");
                retryCount++;
                if (retryCount > 5) {
                    LOG_WARN("Retrying connection...");
                    connectionState = CST_DISCONECTED;
                    retryCount = 0;
                }
                break;
            case 1: 
                connectionState = CST_CONNECTED;
                LOG_INFO("Connected to server"); 
                retryCount = 0;
                break;
            default: 
                connectionState = CST_DISCONECTED; 
                LOG_WARN("Connected faild"); 
                retryCount = 0;
                break;
            }
            break;
        case CST_CONNECTED:
            ChatClient(_conData);   
            LOG_INFO("Thread closed");
            return NULL;
        }
    }

    LOG_INFO("Client - Thread Closed");
}

int ConnectToSocket(struct ConnectionData* _conData){

    // connect() connects asks the port for connection, if there is a 
    // server listening and it accepts he connection is successfull.
    if(connect(_conData->sockfd, (SA*)&_conData->addr, sizeof(_conData->addr)) != 0){
        int err = WSAGetLastError();
        if(err == WSAEWOULDBLOCK || err == WSAEINPROGRESS){
            // trying to connect to server 
            return 1;
        } else {
            SetErrnoFromWinsock();
            LOG_ERROR("Connection with server faild");
            return 0;
        }
    } else {
        LOG_INFO("Connected to server");
        return 1;
    }

    return 0;
};

int CheckSocketWriteable(struct ConnectionData* _conData){

    fd_set writefds; // Struct for remembering monitor writability
    FD_ZERO(&writefds); // Init fds to empty
    FD_SET(_conData->sockfd, &writefds); // adds the socket to write set, meaning selcet will monitor this socket for writability. 
   
    // Sets timeout (check time=
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // Acutally performs the check on the socket, 0 = irrelevant, NULL for read cheack, fds for write check, NULL for error check
    int result = select(_conData->sockfd + 1, NULL, &writefds, NULL, &timeout);
    // returns num ready sockets, FD_ISSET makes sure our socket is ready
    if (result < 0) {
        // select() failed
        SetErrnoFromWinsock();
        LOG_ERROR("select() faild");
        return -1;
    } else if (result == 0) {
        // Socket not writeable yet (still connecting or idle)
        return 0;
    } else if (FD_ISSET(_conData->sockfd, &writefds)) {
        int optval;
        int optlen = sizeof(optval);
        if (getsockopt(_conData->sockfd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen) < 0) {
            SetErrnoFromWinsock();
            LOG_ERROR("getSockopt() faild");
            return -2;
        }
        if (optval == 0) {
            // Socket is writeable and connected
            return 1;
        } else {
            // Connection failed, return error code
            return optval;
        }
    }

    // Should not reach here
    return -3;
}

int AcceptConection(struct ConnectionData* _conData){

    // 5. Accept the data packet from client
    _conData->connfd = accept(_conData->sockfd, (SA*)&_conData->addr, (int*)&_conData->addr_len);
    if(_conData->connfd == INVALID_SOCKET) {
        int err = WSAGetLastError();
        if(err == WSAEWOULDBLOCK){
            // No client yet, continue with other things
        } else {
            SetErrnoFromWinsock();
            LOG_ERROR("Server accept faild");
            return 0;
        }
    
    } else {
        LOG_INFO("Server acceped client");
        return 1;
    }

    return 0;
};
 
void ChatServer(struct ConnectionData* _conData){
    char msgBuffer[MAX];
    int n;

    // Infinate loop for chating 
    while(1)
    {
        memset(msgBuffer, 0, sizeof(msgBuffer));
        
        // Read message from client and copy to buffer;
        recv(_conData->connfd, msgBuffer, sizeof(msgBuffer),0);
        LOG_INFO("From client: %s\t To client : ", msgBuffer); 
        memset(msgBuffer, 0, sizeof(msgBuffer));
        n = 0;

        // Sends reply to user 
        fgets(msgBuffer, sizeof(msgBuffer), stdin);
        send(_conData->connfd, msgBuffer, sizeof(msgBuffer),0);

        // if msg contains "Exit" then server exit and chat ended.
        if(strncmp("exit", msgBuffer, 4) == 0){
            LOG_INFO("Chat ended, server shutting down");
            break;
        }
    }   

};

void ChatClient(struct ConnectionData* _conData){

    char msgBuffer[MAX];
    int n;

   // Infinate loop for chating 
    while(1){
        memset(msgBuffer, 0, sizeof(msgBuffer));

        // Write to buffer
        LOG_INFO("Send to server : ");
        n = 0;

        // Gets the users input
        fgets(msgBuffer, sizeof(msgBuffer), stdin);

        // sendes buffer to server
        send(_conData->sockfd, msgBuffer, sizeof(msgBuffer),0);
        
        // clears buffer
        memset(msgBuffer, 0, sizeof(msgBuffer));
        // reads the response from the server
        recv(_conData->sockfd, msgBuffer, sizeof(msgBuffer),0);
        LOG_INFO("From server : %s", msgBuffer);

        // if msg contains "Exit" then server exit and chat ended.
        if((strncmp(msgBuffer, "exit", 4)) == 0){
            LOG_INFO("Chat ended, client shutting down");
            break;
        }

    }

};

int CleanupNetwork(struct ConnectionData** _conData){
    int exit = 0; 

    if(connectionSide != CSI_NON){
         if(closesocket((*_conData)->sockfd) == SOCKET_ERROR){
            SetErrnoFromWinsock();
            LOG_ERROR("Close Socket Faild");
            exit = 1;
        }
         if(WSACleanup() == SOCKET_ERROR){
            SetErrnoFromWinsock();
            LOG_ERROR("WSA Cleanup Fild");
            exit = 1;
        } 
    }

    if(*_conData){
        free(*_conData);
        *_conData = NULL;
    }
   
    return exit;
}

void SetErrnoFromWinsock() {
    int wsa_err = WSAGetLastError();
    switch (wsa_err) {
        case WSAEACCES: errno = EACCES; break;
        case WSAEADDRINUSE: errno = EADDRINUSE; break;
        case WSAEADDRNOTAVAIL: errno = EADDRNOTAVAIL; break;
        case WSAEAFNOSUPPORT: errno = EAFNOSUPPORT; break;
        case WSAECONNREFUSED: errno = ECONNREFUSED; break;
        case WSAECONNRESET: errno = ECONNRESET; break;
        case WSAEINTR: errno = EINTR; break;
        case WSAEINVAL: errno = EINVAL; break;
        case WSAEISCONN: errno = EISCONN; break;
        case WSAEMFILE: errno = EMFILE; break;
        case WSAENETDOWN: errno = ENETDOWN; break;
        case WSAENETRESET: errno = ENETRESET; break;
        case WSAENETUNREACH: errno = ENETUNREACH; break;
        case WSAENOBUFS: errno = ENOBUFS; break;
        case WSAENOTCONN: errno = ENOTCONN; break;
        case WSAENOTSOCK: errno = ENOTSOCK; break;
        case WSAETIMEDOUT: errno = ETIMEDOUT; break;
        default: errno = EIO; break;
    }
}
