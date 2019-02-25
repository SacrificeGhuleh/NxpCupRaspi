//
// Created by Richard Zvonek on 01-Dec-18.
//

#ifndef NXPCUPRASPI_NXPSERIALPORT_H
#define NXPCUPRASPI_NXPSERIALPORT_H


#if defined(__linux__ ) || defined(WIN32)


#include "NxpDefines.h"
#include "SerialPort.h"

namespace nxpbc {
    class NxpSerialPort : public SerialPort {
    public:
        NxpSerialPort();

        /**
         * @brief Metoda pro získání dat ze sériové linky
         * @return true, pokud byla data přečtena správně
         */
        bool receiveComData(unsigned char (&dataBuffer)[DATA_BUFF_SIZE],
                            unsigned char (&serialDataBuffer)[SERIAL_DATA_BUFF_SIZE]);

        /**
         * @brief Odešle nastavení ovládání přes sériovou linku
         * @return true, pokud byla data odeslána správně
         */
        bool sendComControl(unsigned char (&serialControlBuffer)[SERIAL_CONTROL_BUFF_SIZE]);

        /**
         * @brief Odešle nastavení parametrů přes sériovou linku
         * @return true, pokud byla data odeslána správně
         */
        bool sendComSetting(unsigned char (&serialSettingBuffer)[SERIAL_SETTING_BUFF_SIZE]);

        virtual ~NxpSerialPort() override;
    };
}

#endif
#endif //NXPCUPRASPI_NXPSERIALPORT_H
