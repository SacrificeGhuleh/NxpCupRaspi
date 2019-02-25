//
// Created by zvone on 20-Feb-19.
//

#ifndef NXPCUPRASPI_LINUXUDPSERVER_H
#define NXPCUPRASPI_LINUXUDPSERVER_H

#if defined(__linux__) || defined(WIN32)

#include "UDPServer.h"

namespace nxpbc {
    class LinuxUDPServer : UDPServer {
    public:
        LinuxUDPServer(unsigned int port);

        virtual int receiveUdpData();

        virtual bool sendUdpData(unsigned char *data, int count);

        virtual ~LinuxUDPServer();

    private :
        int udpFd_;
        struct sockaddr_in udpMyaddr_;    /* our address */
        struct sockaddr_in udpRemaddr_;    /* remote address */
        socklen_t udpAddrlen_;        /* length of addresses */
    };
}

#endif
#endif //NXPCUPRASPI_LINUXUDPSERVER_H
