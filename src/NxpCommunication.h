//
// Created by Richard Zvonek on 01-Dec-18.
//

#ifndef NXPCUPRASPI_NXPCOMMUNICATION_H
#define NXPCUPRASPI_NXPCOMMUNICATION_H

#if defined(__linux__) || defined(WIN32)

#include "NxpDefines.h"
#include "NxpSerialPort.h"
#include "LinuxUDPServer.h"

struct s_setting;
struct s_control;
struct s_data;

namespace nxpbc {
    class NxpCommunication : public NxpSerialPort, public LinuxUDPServer {

    public:
        NxpCommunication();

        /**
         * @brief Převede strukturu s_control do bytového pole pro odeslání
         * @param control Nastavení ovládání
         */
        void setControl(s_control *control);

        /**
         * @brief Převede strukturu s_setting do bytového pole pro odeslání
         * @param setting Nastavení parametrů
         */
        void setSetting(s_setting *setting);

        /**
         * @brief Metoda pro odeslání řídících parametrů přes sériovou linku
         */
        void sendComControl();

        /**
         * @brief Metoda pro odeslání nastavení přes sériovou linku.
         */
        void sendComSetting();

        /**
         * @brief Metoda pro přijetí sériových dat.
         * @return True, pokud byla data přijata správně.
         */
        bool receiveComData();

        /**
         * @brief Metoda pro získání struktury s_data ze sériových dat.
         * @return struktura s_data ze sériových dat.
         */
        s_data *getSData();

        /**
         * @brief Metoda pro odeslání dat přes UDP.
         * @return True, pokud byla data odeslána správně.
         */
        bool sendUdpData();

        virtual ~NxpCommunication() override;

    private:
        /**
         * @brief Buffer dat
         */
        unsigned char dataBuffer[DATA_BUFF_SIZE] = {0};

        /**
         * @brief Sériový buffer dat
         */
        unsigned char serialDataBuffer[SERIAL_DATA_BUFF_SIZE] = {
                STX, 0x19, 0x01, CMD_DATA,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                ETX
        };
        /**
         * @brief Sériový buffer nastavení ovládání
         */
        unsigned char serialControlBuffer[SERIAL_CONTROL_BUFF_SIZE] = {
                STX, 0x11, 0x00, CMD_CONTROL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ETX
        };

        /**
         * @brief Sériový buffer nastavení
         */
        unsigned char serialSettingBuffer[SERIAL_SETTING_BUFF_SIZE] = {
                STX, 0x11, 0x00, CMD_SETTING, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ETX
        };
    };
}

#endif
#endif //NXPCUPRASPI_NXPCOMMUNICATION_H
