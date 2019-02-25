//
// Created by Richard Zvonek on 01-Dec-18.
//

#if defined(__linux__ ) || defined(WIN32)

#include "pch.h"
#include "NxpCommunication.h"


namespace nxpbc {

    NxpCommunication::NxpCommunication() :
            NxpSerialPort(),
            LinuxUDPServer(2222) {
    }

    void NxpCommunication::setControl(s_control *control) {
        char *buff = reinterpret_cast<char *>(control);
        std::copy(buff, buff + 12, std::begin(serialControlBuffer) + 4);
    }

    void NxpCommunication::setSetting(s_setting *setting) {
        char *buff = reinterpret_cast<char *>(setting);
        std::copy(buff, buff + 12, std::begin(serialSettingBuffer) + 4);

    }

    void NxpCommunication::sendComSetting() {
        NxpSerialPort::sendComSetting(serialSettingBuffer);
    }

    void NxpCommunication::sendComControl() {
        NxpSerialPort::sendComControl(serialControlBuffer);

    }

    bool NxpCommunication::receiveComData() {
        return NxpSerialPort::receiveComData(dataBuffer, serialDataBuffer);
    }

    s_data *NxpCommunication::getSData() {
        return reinterpret_cast<s_data *>(&dataBuffer);
    }

    bool NxpCommunication::sendUdpData() {
        bool ret = true;
        ret &= LinuxUDPServer::sendUdpData(serialControlBuffer, sizeof(serialControlBuffer));
        ret &= LinuxUDPServer::sendUdpData(serialSettingBuffer, sizeof(serialSettingBuffer));
        ret &= LinuxUDPServer::sendUdpData(serialDataBuffer, sizeof(serialDataBuffer));
        return ret;
    }

    NxpCommunication::~NxpCommunication() {

    }
}

#endif
