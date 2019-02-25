//
// Created by Richard Zvonek on 01-Dec-18.
//


#if defined(__linux__ ) || defined(WIN32)

#include "pch.h"
#include "NxpSerialPort.h"
#include "Settings.h"
#include "Logger.h"

namespace nxpbc {

    NxpSerialPort::NxpSerialPort() : SerialPort(COM_PORT_NAME, COM_BAUD_RATE) {
    }

    bool NxpSerialPort::receiveComData(unsigned char (&dataBuffer)[DATA_BUFF_SIZE],
                                       unsigned char (&serialDataBuffer)[SERIAL_DATA_BUFF_SIZE]) {
        std::list<char> list;
        /**
         * @brief Citac pro timeout, ochrana proti zacykleni
         */
        int timeoutCounter = 0;
        NXP_TRACE("Cekam na data");
        /*
         * Smyska pro hledani zacatku dat
         * */
        while (true) {

            /*
             * Cekej, dokud nemas k dispozici dostatek dat
             * */
            while (dataAvailCom() < SERIAL_DATA_BUFF_SIZE) {
                if (timeoutCounter++ > COM_TIMEOUT) {
                    NXP_ERROR("\nProblem se ctenim dat - timeout"
                                      NL);
                    return false;
                }
#if NXP_PRINT_TRACE
                printf(".");
#endif
                delay(1);
            }

            list.emplace_back(static_cast<char>(getcharCom()));
            if (list.size() < 4) {
                continue;
            }

            std::list<char>::iterator it = list.begin();

            if ((*(it++) == STX) &&
                (*(it++) == 0x19) &&
                (*(it++) == 0x01) &&
                (*it == CMD_DATA)) {
#if NXP_PRINT_TRACE
                printf("" NL);
#endif
                NXP_TRACE("Nalezen zacatek dat"
                                  NL);
                break;
            } else {
                list.pop_front();
            }
#if NXP_PRINT_TRACE
            printf("*");
#endif
        }

        /*
         * Cekej, dokud nemas k dispozici dostatek dat
         * */
        while (dataAvailCom() < DATA_BUFF_SIZE + 1) {
            if (timeoutCounter++ > COM_TIMEOUT) {
                NXP_ERROR("Problem se ctenim dat - timeout"
                                  NL);
                return false;
            }
#if NXP_PRINT_TRACE
            printf(".");
#endif
            delay(1);
        }
//#if NXP_PRINT_TRACE
//        printf("" NL);
//#endif
        for (auto it = std::begin(dataBuffer); it != std::end(dataBuffer); ++it) {
            *it = static_cast<unsigned char>(getcharCom());
        }

        if (static_cast<char>(getcharCom()) == ETX) {
            NXP_TRACE("Nalezen konec dat"
                              NL);

            /*
             * Kvuli UDP posilani
             * */
            std::copy(std::begin(dataBuffer), std::end(dataBuffer), std::begin(serialDataBuffer) + 4);
            return true;
        } else {
            NXP_ERROR("Data prectena spatne"
                              NL);
            return false;
        }
    }

    bool NxpSerialPort::sendComControl(unsigned char (&serialControlBuffer)[SERIAL_CONTROL_BUFF_SIZE]) {
        errno = 0;
        for (const unsigned char &ch : serialControlBuffer) {
            putcharCom(ch);
        }
        if (errno) {
            NXP_ERRORP("Chyba: %s"
                               NL, strerror(errno));
            errno = 0;
            return false;
        }
        return true;
    }

    bool NxpSerialPort::sendComSetting(unsigned char (&serialSettingBuffer)[SERIAL_SETTING_BUFF_SIZE]) {
        NXP_TRACE("Sending setting");
        errno = 0;
        for (const unsigned char &ch : serialSettingBuffer) {
            errno += static_cast<int>(putcharCom(ch));
        }
        if (errno) {
            NXP_ERRORP("Chyba: %s"
                               NL, strerror(errno));
            errno = 0;
            return false;
        }
        return true;
    }

    NxpSerialPort::~NxpSerialPort() = default;
}
#endif