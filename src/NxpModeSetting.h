//
// Created by zvone on 24-Apr-19.
//

#ifndef NXPCUPRASPI_NXPMODESETTING_H
#define NXPCUPRASPI_NXPMODESETTING_H

#include "NxpDefines.h"

namespace nxpbc {
    struct NxpModeSetting {
        NxpModeSetting(float regulatorP, float regulatorI, float regulatorD, float diffCoef, uint16_t maxSpeed) :
                regulatorP(regulatorP),
                regulatorI(regulatorI),
                regulatorD(regulatorD),
                diffCoef(diffCoef),
                maxSpeed(maxSpeed) {}

        float regulatorP = 0.0f;
        float regulatorI = 0.0f;
        float regulatorD = 0.0f;
        float diffCoef = 0.0f;
        uint16_t maxSpeed = 0.0f;


        bool operator==(const NxpModeSetting &rhs) {
            return this->regulatorP == rhs.regulatorP &&
                   this->regulatorI == rhs.regulatorI &&
                   this->regulatorD == rhs.regulatorD &&
                   this->diffCoef == rhs.diffCoef;
        }

        bool operator!=(const NxpModeSetting &rhs) {
            return !(*this == rhs);
        }


    };

//
//#define CONST_ERROR 160.6f
//#define CONST_DERIVATIVE 8.3f
//#define CONST_INTEGRAL 0.5f
//#define TURN_CONTROL_COEFICIENT 1.28f
//#define CONTROL_PWM_MAX 300

    const NxpModeSetting constRide1Setting = NxpModeSetting(
            CONST_ERROR,             // PID error
            CONST_INTEGRAL,          // PID integral
            CONST_DERIVATIVE,        // PID Derivation
            TURN_CONTROL_COEFICIENT, // DIFF coefficient
            CONTROL_PWM_MAX);        // PWM base speed

    const NxpModeSetting constRide2Setting = NxpModeSetting(
            CONST_ERROR,             // PID error
            CONST_INTEGRAL,          // PID integral
            CONST_DERIVATIVE,        // PID Derivation
            15.f,                    // DIFF coefficient
            270);                    // PWM base speed

    const NxpModeSetting constRide3Setting = NxpModeSetting(
            CONST_ERROR,// PID error
            CONST_INTEGRAL,// PID integral
            CONST_DERIVATIVE,// PID Derivation
            TURN_CONTROL_COEFICIENT,// DIFF coefficient
            CONTROL_PWM_MAX);   // PWM base speed

    const NxpModeSetting constFigure8Setting = NxpModeSetting(
            200,  // PID error
            8,    // PID integral
            0,    // PID Derivation
            30.f, // DIFF coefficient
            250); // PWM base speed

}

#endif //NXPCUPRASPI_NXPMODESETTING_H
