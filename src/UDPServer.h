//
// Created by Richard Zvonek on 19-Nov-18.
//

#ifndef NXPCUPRASPI_UDPSERVER_H
#define NXPCUPRASPI_UDPSERVER_H


#ifdef _WIN32
//typedef int socklen_t;

#include <winsock.h>

#else
//#include <netdb.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
#endif

//#include <tfc.h>

struct sockaddr_in;
typedef unsigned int socklen_t;

namespace nxpbc {


#define UDP_BUFSIZE 2048

    /**
     * @brief Třída pro UDP server pro Raspberry Pi
     */
    class UDPServer {
    public:
        UDPServer(unsigned int port);

        virtual int receiveUdpData() = 0;

        virtual bool sendUdpData(unsigned char *data, int count) = 0;

        virtual ~UDPServer() = default;

    protected:
        const unsigned int udpPort_;
        char udpBuf_[UDP_BUFSIZE];
    };
}
#endif //NXPCUPRASPI_UDPSERVER_H
