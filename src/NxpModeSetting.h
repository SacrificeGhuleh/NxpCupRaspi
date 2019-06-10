//
// Created by zvone on 24-Apr-19.
//

#ifndef NXPCUPRASPI_NXPMODESETTING_H
#define NXPCUPRASPI_NXPMODESETTING_H

#include "NxpDefines.h"
#include "NxpModes.h"

namespace nxpbc {
    /**
     * @brief Nastavení jednotlivých módů
     */
    struct NxpModeSetting {
        NxpModeSetting(float regulatorP, float regulatorI, float regulatorD, float diffCoef, uint16_t maxSpeed,
                       NxpModes mode) :
                regulatorP_(regulatorP),
                regulatorI_(regulatorI),
                regulatorD_(regulatorD),
                diffCoef_(diffCoef),
                maxSpeed_(maxSpeed),
                mode_{(uint8_t) mode} {}

        float regulatorP_ = 0.0f;
        float regulatorI_ = 0.0f;
        float regulatorD_ = 0.0f;
        float diffCoef_ = 0.0f;
        uint16_t maxSpeed_ = 0.0f;
        /*NxpModes*/ uint8_t mode_ = NxpModes::modeRideOne;


        bool operator==(const NxpModeSetting &rhs) {
            return this->regulatorP_ == rhs.regulatorP_ &&
                   this->regulatorI_ == rhs.regulatorI_ &&
                   this->regulatorD_ == rhs.regulatorD_ &&
                   this->diffCoef_ == rhs.diffCoef_ &&
                   this->mode_ == rhs.mode_;
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
            CONTROL_PWM_MAX,         // PWM base speed
            NxpModes::modeRideOne);  // Ride mode

    const NxpModeSetting constRide2Setting = NxpModeSetting(
            CONST_ERROR,             // PID error
            CONST_INTEGRAL,          // PID integral
            CONST_DERIVATIVE,        // PID Derivation
            110,                     // DIFF coefficient
            280/*350*/,                     // PWM base speed
            NxpModes::modeRideTwo);  // Ride mode

    const NxpModeSetting constRide3Setting = NxpModeSetting(
            CONST_ERROR,               // PID error
            CONST_INTEGRAL,            // PID integral
            CONST_DERIVATIVE,          // PID Derivation
            450,                        // DIFF coefficient
			CONTROL_PWM_MAX,                       // PWM base speed
            NxpModes::modeRideThree);  // Ride mode

    const NxpModeSetting constFigure8Setting = NxpModeSetting(
            250,                       // PID error
            0,                         // PID integral
            0.1f,                      // PID Derivation
            0,                         // DIFF coefficient
            250,                       // PWM base speed
            NxpModes::modeFigEight);   // Ride mode

    const NxpModeSetting constSpeedZoneSetting = NxpModeSetting(
            250,                       // PID error
            8,                         // PID integral
            0,                         // PID Derivation
            100,                       // DIFF coefficient
            CONTROL_PWM_MAX,           // PWM base speed
            NxpModes::modeSpeedZone);  // Ride mode

    const NxpModeSetting constObstacleSetting = NxpModeSetting(
            200,                       // PID error
            8,                         // PID integral
            0,                         // PID Derivation
            30,                        // DIFF coefficient
            250,                       // PWM base speed
            NxpModes::modeObstacle);   // Ride mode

}

#endif //NXPCUPRASPI_NXPMODESETTING_H
