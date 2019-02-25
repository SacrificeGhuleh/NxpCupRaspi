//
// Created by Richard Zvonek on 01-Dec-18.
//


#if defined(__linux__ ) || defined(WIN32)


#include "pch.h"
#include "SerialPort.h"
#include "Logger.h"

namespace nxpbc {

    SerialPort::SerialPort(const char *comPortName, unsigned int comBaudRate) :
            comPortName_{comPortName},
            comBaudRate_{comBaudRate},
            comOpened_{false} {
    }

    bool SerialPort::isComOpen() {
        return comOpened_ && (comFd >= 0);
    }

    bool SerialPort::resetCom() {
        if (!comOpened_) return false;
        NXP_WARN("Resetuji seriovy port");
        serialClose(comFd);
        comFd = serialOpen(comPortName_, comBaudRate_);
        serialFlush(comFd);
        return isComOpen();
    }

    bool SerialPort::openCom() {
        if (comOpened_) return false;
        comOpened_ = true;
        comFd = serialOpen(comPortName_, comBaudRate_);
        return isComOpen();
    }

    bool SerialPort::closeCom() {
        if (!comOpened_) return false;
        comOpened_ = false;
        serialClose(comFd);
        return !isComOpen();

    }

    bool SerialPort::flushCom() {
        if (!comOpened_) return false;
        serialFlush(comFd);
        return true;
    }

    bool SerialPort::putcharCom(const unsigned char c) {
        if (!comOpened_) return false;
        serialPutchar(comFd, c);
        return true;
    }

    bool SerialPort::putsCom(const char *s) {
        if (!comOpened_) return false;
        serialPuts(comFd, s);
        return true;
    }

    int SerialPort::dataAvailCom() {
        if (!comOpened_) return -1;
        return serialDataAvail(comFd);
    }

    int SerialPort::getcharCom() {
        if (!comOpened_) return -1;
        return static_cast<uint8_t >(serialGetchar(comFd));
    }

    SerialPort::~SerialPort() {
        closeCom();
    }
}

#endif