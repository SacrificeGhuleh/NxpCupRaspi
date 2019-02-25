//
// Created by zvone on 20-Feb-19.
//

#if defined(__linux__) || defined(WIN32)

#include "pch.h"
#include "LinuxUDPServer.h"

namespace nxpbc {

    int LinuxUDPServer::receiveUdpData() {
        int recvlen = recvfrom(udpFd_, udpBuf_, UDP_BUFSIZE, 0, (struct sockaddr *) &udpRemaddr_, &udpAddrlen_);
        if (recvlen > 0) {
            udpBuf_[recvlen] = 0;
            printf("received message: \"%s\" (%d bytes)" NL, udpBuf_, recvlen);
        } else
            printf("uh oh - something went wrong!" NL);
        return 0;
    }

    bool LinuxUDPServer::sendUdpData(unsigned char *data, int count) {
        return sendto(udpFd_, reinterpret_cast<char *>(data), count, 0, (struct sockaddr *) &udpRemaddr_, udpAddrlen_) <
               0;
    }

    LinuxUDPServer::LinuxUDPServer(unsigned int port) : UDPServer(port) {

        udpAddrlen_ = sizeof(udpRemaddr_);

        /* create a UDP socket */
        if ((udpFd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("cannot create socket" NL);
            return;
        }

        /*For non blockinng reading*/
        /*int flags = fcntl(udpFd_, F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(udpFd_, F_SETFL, flags);*/

        /* bind the socket to any valid IP address and a specific port */
        memset((char *) &udpMyaddr_, 0, sizeof(udpMyaddr_));
        udpMyaddr_.sin_family = AF_INET;
        udpMyaddr_.sin_addr.s_addr = htonl(INADDR_ANY);
        udpMyaddr_.sin_port = htons(udpPort_);

        if (bind(udpFd_, (struct sockaddr *) &udpMyaddr_, sizeof(udpMyaddr_)) < 0) {
            perror("bind failed");
        }
    }

    LinuxUDPServer::~LinuxUDPServer() {
        close(udpFd_);
    }

}

#endif