//
// Created by zvone on 20-Feb-19.
//

#ifndef NXPCUPRASPI_FREESCALEUDPSERVER_H
#define NXPCUPRASPI_FREESCALEUDPSERVER_H


#include "UDPServer.h"

namespace nxpbc {


    class FreescaleUdpServer : UDPServer {
    public:
        FreescaleUdpServer(unsigned int port);

    private:
        int receiveUdpData() override;

        bool sendUdpData(unsigned char *data, int count) override;

    public:
        virtual ~FreescaleUdpServer();
    };

}


#endif //NXPCUPRASPI_FREESCALEUDPSERVER_H
