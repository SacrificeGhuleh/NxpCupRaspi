//
// Created by Richard Zvonek on 01-Dec-18.
//


#if defined(__linux__ ) || defined(WIN32)


#ifndef NXPCUPRASPI_SERIALPORT_H
#define NXPCUPRASPI_SERIALPORT_H

namespace nxpbc {

    class SerialPort {
    public:
        SerialPort(const char *comPortName, unsigned int comBaudRate);

        bool isComOpen();

        bool resetCom();

        bool openCom();

        bool closeCom();

        bool flushCom();

        bool putcharCom(unsigned char c);

        bool putsCom(const char *s);

        int dataAvailCom();

        int getcharCom();

        virtual ~SerialPort();

    private:
        bool comOpened_;
        const char *comPortName_;
        unsigned int comBaudRate_;

        int comFd;

    };
}

#endif
#endif //NXPCUPRASPI_SERIALPORT_H
