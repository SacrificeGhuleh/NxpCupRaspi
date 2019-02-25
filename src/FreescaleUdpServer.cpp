//
// Created by zvone on 20-Feb-19.
//

#include "FreescaleUdpServer.h"

nxpbc::FreescaleUdpServer::FreescaleUdpServer(unsigned int port) : UDPServer(port) {}

nxpbc::FreescaleUdpServer::~FreescaleUdpServer() {

}

int nxpbc::FreescaleUdpServer::receiveUdpData() {
    return 0;
}

bool nxpbc::FreescaleUdpServer::sendUdpData(unsigned char *data, int count) {
    return false;
}
