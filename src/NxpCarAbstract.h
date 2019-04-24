//
// Created by Richard Zvonek on 01-Dec-18.
//

#ifndef NXPCUPRASPI_NXPCAR_H
#define NXPCUPRASPI_NXPCAR_H

#include "NxpDefines.h"
#include "NxpSendFrame.h"
#include "NxpModeSetting.h"

class TFC;

class PID_new;

//struct SendData;

namespace nxpbc {
    enum class MotorsState;

    class LineTracer;

    class NxpCommunication;

    class PID;

    /**
     * @brief Hlavní metoda pro NXP autíčko.
     */
    class NxpCarAbstract {
    public:

        /**
         * @brief Konstruktor třídy
         */
        NxpCarAbstract();

        /**
         * @brief Metoda pro řízení auta v nekonečné smyčce
         */
        virtual void update() = 0;

        /**
         * @brief Metoda pro získání aktuálního stavu jízdy autíčka.
         * @return True, pokud se má opakovat metoda update()
         */
        virtual bool isRunning();

        virtual ~NxpCarAbstract();

        bool shouldReset();

        TFC *getTfc();

        SendData sendData_;
    protected:

        /**
         * @brief bity pro indikaci zmeny serva
         * 0b0000 0001  INCREASING - servo zvysuje natoceni
         * 0b0000 0010  DECREASING - servo snizuje natoceni
         * 0b0000 0000  STAYING - servo zachovava pozici
         */

        uint8_t servoIncreasingBits_;

        uint8_t prevZonesFoundCount_;
        bool inSpeedCheckZone_;
        uint8_t speedCheckZoneDebounce_;
        MotorsState motorsState_;
        bool running_;
        bool shouldReset_;
        uint16_t motorSpeed_;
        //int leftSpeed_;
        //int rightSpeed_;
        const bool servoChannel_;
        int16_t prevServoPos_;
        int16_t servoPos_;
        bool debounce_;
        uint8_t debounceCounter_;
        const uint8_t debounceCounterMax_;
        uint8_t btns_;
        uint16_t timestamp_;
        uint16_t timestampDiff_;
        uint8_t left_;
        uint8_t right_;
        int startTimer_;
        //uint16_t image_[CAMERA_LINE_LENGTH];
        float steerSetting_;

        LineTracer *tracer_;
        PID *pid_;
        TFC *tfc_;
        PID_new *steerRegulator_;
        double steerRegulatorInput_;
        double steerRegulatorOutput_;
        double steerRegulatorTarget_;
        NxpModeSetting modeSetting_;
        /**
         * @brief Metoda pro výpis aktuálního stavu autíčka do konzole.
         */
        virtual void printCurrentState();

        /**
         * @brief Metoda pro ovládání tlačítek na autíčku
         * @param buttons Stav tlačítek
         */
        virtual void handleBtns(unsigned char buttons) = 0;

        /**
         * @brief Metoda pro předání parametrů jízdy autíčku.
         */
        virtual void setRide();

//
//        /**
//         * @brief Metoda pro řízení serva pomocí hodnoty PID regulátoru
//         * @param pidValue Hodnota vypočtená PID regulátorem
//         */
//        virtual void steer(float pidValue);


        /**
         * @brief Metoda pro ořez poměrů stran.
         * @param ratioDiff Rozdíl poměrů vzdáleností od čáry.
         */
        virtual void clipRatio(float ratioDiff);

        /**
         * @brief Metoda pro rozjezd autíčka.
         */
        virtual void start();

        virtual float servoToAngle();


        virtual void resetRegulator();
        virtual void handleReset();

    };
}

#endif //NXPCUPRASPI_NXPCAR_H
