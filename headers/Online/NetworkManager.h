#pragma once

#include "Logger.h"

#include <winsock.h>
#include <errno.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

struct ConnectionData{
    struct sockaddr_in addr; // connection address
    size_t addr_len;   // generall used for the size of an address in relation to accepting or reciving. 
    int sockfd;     // means socket file descriptor (socket ID)
    int connfd;     // means connection file descriptor (represents a new socket ID spesific for a connected client)
}; 

extern enum ConnectionSide{
    CSI_SERVER,
    CSI_CLIENT,
    CSI_NON
} connectionSide;

extern enum ConnectionState{
    CST_CONNECTING,
    CST_DISCONECTED,
    CST_CONNECTED
} connectionState;

int InitNetwork(int* _argc, char *argv[], struct ConnectionData* _conData);
int CreateSocket(struct ConnectionData* _conData);
void AssignIpPortLong(struct ConnectionData* _conData, int _socketType, u_long _networkInterfaces, int _portId);
void AssignIpPortChar(struct ConnectionData* _conData, int _socketType, const char* _networkInterfaces, int _portId);
int BindPortToAddress(struct ConnectionData* _conData);
void* ServerListen(void* _arg);
void* ClientConnect(void* _arg);
int ConnectToSocket(struct ConnectionData* _conData);
int CheckSocketWriteable(struct ConnectionData* _conData);
int AcceptConection(struct ConnectionData* _conData);
void ChatServer(struct ConnectionData* _conData);
void ChatClient(struct ConnectionData* _conData);
int CleanupNetwork(struct ConnectionData** _conData);

void SetErrnoFromWinsock();
