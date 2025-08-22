#include "NetworkManager.h"
#include "client.h"
#include "Logger.h"

int InitClient(struct ConnectionData* _conData){

    // 2. Assign IP & Port
    if(!CreateSocket(_conData))
        return 0;

    // 3. Bind newly created socket to given IP and verificiation.
    // Ip:"127.0.0.1" is the loopback address, also known as local host.
    // and will mean it only searches on ports connected to the same machine.
    // all devices on nettowkr: INADDER_ANY, all availible interfaces: "0.0.0.0"
    AssignIpPortChar(_conData, AF_INET, "127.0.0.1", PORT);

    return 1;
};

