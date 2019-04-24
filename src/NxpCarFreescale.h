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

    class NxpCar : public NxpCarAbstract {
    private:
        void setSendData();

        void handleIrSensors();

    protected:
        virtual void handleBtns(unsigned char buttons) override;

    public:

        //GyroMagSensor *gyroMagSensor_;

        uint8_t lineCrossBits_;
        uint8_t leftLineCrossTimer_;
        uint8_t middleLineCrossTimer_;
        uint8_t rightLineCrossTimer_;
        const uint8_t maxCrossTimer_;

        bool hadLeftInPrevFrame_;
        bool hadRightInPrevFrame_;

        uint16_t leftLineLostTimer_;
        uint16_t rightLineLostTimer_;

        NxpCar();

        virtual ~NxpCar();

        //Enet *enet;

        void update() override;
    };


};

#endif

#endif //NXPCUPRASPI_NXPCARFREESCALE_H
