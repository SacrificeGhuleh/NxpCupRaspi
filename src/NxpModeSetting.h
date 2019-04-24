//
// Created by zvone on 24-Apr-19.
//

#ifndef NXPCUPRASPI_NXPMODESETTING_H
#define NXPCUPRASPI_NXPMODESETTING_H

#include "NxpDefines.h"

namespace nxpbc {
    struct NxpModeSetting {
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

    const NxpModeSetting constRide1Setting = {CONST_ERROR, CONST_INTEGRAL, CONST_DERIVATIVE, TURN_CONTROL_COEFICIENT, CONTROL_PWM_MAX};
    const NxpModeSetting constRide2Setting = {280, CONST_INTEGRAL, CONST_DERIVATIVE, 15.f, CONTROL_PWM_MAX};
    const NxpModeSetting constRide3Setting = {CONST_ERROR, CONST_INTEGRAL, CONST_DERIVATIVE, TURN_CONTROL_COEFICIENT, CONTROL_PWM_MAX};

}

#endif //NXPCUPRASPI_NXPMODESETTING_H
