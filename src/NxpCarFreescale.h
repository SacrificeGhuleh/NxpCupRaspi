//
// Created by zvone on 21-Feb-19.
//

#ifndef NXPCUPRASPI_NXPCARFREESCALE_H
#define NXPCUPRASPI_NXPCARFREESCALE_H


#if defined(__MCUXPRESSO) || 1


#include "NxpCarAbstract.h"

//class Enet;


namespace nxpbc {
    //class GyroMagSensor;

    /**
     * @brief Hlavní třída pro model auta pro FRDM-K66F
     */
    class NxpCar : public NxpCarAbstract {
    private:

        /**
         * @brief Metoda pro nastavení dat pro odesílání přes UDP
         */
        void setSendData();

        /**
         * @brief Metoda zastavení auta v případě přejetí čáry
         */
        void handleIrSensors();

    protected:

        /**
         * @brief Metoda pro ošetření vstupu tlačítek
         * @param buttons Hodnoty tlačítek
         */
        virtual void handleBtns(unsigned char buttons) override;

    public:

        //GyroMagSensor *gyroMagSensor_;

        /**
         * @brief Proměnna pro zakódování informace, jestli auto přejelo čáru.
         */
        uint8_t lineCrossBits_;

        /**
         * @brief Čítač pro zpoždění levého IR senzoru
         */
        uint8_t leftLineCrossTimer_;

        /**
         * @brief Čítač pro zpoždění prostředního IR senzoru
         */
        uint8_t middleLineCrossTimer_;

        /**
         * @brief Čítač pro zpoždění pravého IR senzoru
         */
        uint8_t rightLineCrossTimer_;

        /**
         * @brief Maximální hodnota čítače zpoždění IR senzoru
         */
        const uint8_t maxCrossTimer_;

        /**
         * @brief Informace, jestli v předchozím snímku byla vidět levá čára
         */
        bool hadLeftInPrevFrame_;


        /**
         * @brief Informace, jestli v předchozím snímku byla vidět levá čára
         */
        bool hadRightInPrevFrame_;


        /**
         * @brief Čítač ztracené levé čáry
         */
        uint16_t leftLineLostTimer_;


        /**
         * @brief Čítač ztracené pravé čáry
         */
        uint16_t rightLineLostTimer_;

        NxpCar();

        virtual ~NxpCar();

        //Enet *enet;

        /**
         * @brief Metoda pro aktualizace stavu auta
         */
        void update() override;
    };


};

#endif

#endif //NXPCUPRASPI_NXPCARFREESCALE_H
