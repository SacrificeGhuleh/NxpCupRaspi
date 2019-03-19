//
// Created by Richard Zvonek on 01-Dec-18.
//

#include "pch.h"
#include "NxpCarAbstract.h"
#include "Settings.h"
#include "motorsState.h"
#include "LineTracer.h"
#include "Logger.h"
#include "PID.h"
#include "NxpCommunication.h"
#include "Region.h"
#include "PID_New.h"

namespace nxpbc {

    NxpCarAbstract::NxpCarAbstract() :
            motorsState_{nxpbc::MotorsState::Stay},
            running_{true},
            motorSpeed_{0},
            //leftSpeed_{0},
            //rightSpeed_{0},
            servoChannel_{0},
            servoPos_{0},
            debounce_{false},
            debounceCounterMax_{10},
            debounceCounter_{0},
            btns_{0b00},
            timestamp_{0},
            timestampDiff_{0},
            left_{Region::minLeft},
            right_{Region::maxRight},
            //image_{0},
            steerSetting_{0.f},
            tracer_{new LineTracer(5)},
            pid_{new PID(CONST_ERROR, CONST_INTEGRAL, CONST_DERIVATIVE, 1000.f)},
            steerRegulatorInput{0.},
            steerRegulatorOutput{0.},
            steerRegulatorTarget{0.},
            steerRegulator{new PID_new(&steerRegulatorInput, &steerRegulatorOutput, &steerRegulatorTarget, CONST_ERROR,
                                       CONST_INTEGRAL, CONST_DERIVATIVE, P_ON_E, DIRECT)},
            tfc_{new TFC()} {

        steerRegulator->SetOutputLimits(0., 1000.);
        steerRegulator->SetSampleTime(10);


        tfc_->setPWMMax(static_cast<uint32_t>(CONTROL_DRIVE_MAX));
        tfc_->setServoCalibration(0b00, static_cast<uint32_t>(SERVO_CENTER), static_cast<uint32_t>(SERVO_LR));
        tfc_->setServoCalibration(0b01, static_cast<uint32_t>(SERVO_CENTER), static_cast<uint32_t>(SERVO_LR));


        const std::string time = std::to_string(std::time(nullptr));
        NXP_TRACEP("Time: %s"
                           NL, time.c_str());
    }

    bool NxpCarAbstract::isRunning() {
        return running_;
    }

    void NxpCarAbstract::printCurrentState() {
        std::string state;
        switch (motorsState_) {
            case nxpbc::MotorsState::Stay:state = FRED("nxpbc::MotorsState::Stay");
                break;
            case nxpbc::MotorsState::Start:state = FYEL("nxpbc::MotorsState::Start");
                break;
            case nxpbc::MotorsState::Ride:state = FGRN("nxpbc::MotorsState::Ride");
                break;
        }
        NXP_TRACEP("State: %s"
                           NL, state.c_str());
    }

    void NxpCarAbstract::setRide() {
        uint16_t leftSpeed = 0, rightSpeed = 0;

        if (servoPos_ > 1000)
            servoPos_ = 1000;
        if (servoPos_ < -1000)
            servoPos_ = -1000;

        float leftTurningCoeficient = 1 + (servoPos_ / 4000.f);
        float rightTurningCoeficient = 1 - (servoPos_ / 4000.f);

        leftSpeed = static_cast<uint16_t>(motorSpeed_ * tfc_->ReadPot_f(0) * leftTurningCoeficient);
        rightSpeed = static_cast<uint16_t>(motorSpeed_ * tfc_->ReadPot_f(0) * rightTurningCoeficient);


        NXP_INFOP("Current riding info: leftMotor: %03d, rightMotor: %03d, servo: %03d, LTC: %f, RTC: %f"
                          NL,
                  static_cast<int>((float) leftSpeed * tfc_->ReadPot_f(0)),
                  static_cast<int>((float) rightSpeed * tfc_->ReadPot_f(0)),
                  NxpCarAbstract::servoPos_,
                  leftTurningCoeficient,
                  leftTurningCoeficient);

        tfc_->setServo_i(servoChannel_, servoPos_);
        tfc_->setMotorPWM_i(static_cast<int>(leftSpeed),
                            static_cast<int>(rightSpeed));
    }

    /*void NxpCarAbstract::steer(float pidValue) {
        servoPos_ = static_cast<int>(pidValue);
    }*/

    void NxpCarAbstract::clipRatio(float ratioDiff) {
        if (ratioDiff > CONTROL_MAX_RATIO) {
            ratioDiff = CONTROL_MAX_RATIO;
        } else if (ratioDiff < -CONTROL_MAX_RATIO) {
            ratioDiff = -CONTROL_MAX_RATIO;
        }
    }

    void NxpCarAbstract::start() {
        if (motorSpeed_ == CONTROL_PWM_MAX) {
            motorsState_ = nxpbc::MotorsState::Ride;
            return;
        }
        motorSpeed_ += CONTROL_PWM_STEP;
    }

    TFC *NxpCarAbstract::getTfc() {
        return tfc_;
    }

    NxpCarAbstract::~NxpCarAbstract() {
        DELETE_AND_PARK(tfc_);
        DELETE_AND_PARK(tracer_);

        DELETE_AND_PARK(pid_);


    }

    void NxpCarAbstract::handleBtns(unsigned char buttons) {
        debounce_ = true;
#if defined(WIN32) || defined(__linux__)
        if ((buttons & 0b01) && ((buttons & 0b01) != (btns_ & 0b01))) {
            NXP_TRACE(BOLD(FRED("Zmackuto tlacitko A, konec programu"
                              NL)));
            running_ = false;
        }
#endif
        if ((buttons & 0b10) && ((buttons & 0b10) != (btns_ & 0b10))) {
            NXP_TRACE(BOLD(FRED("Zmackuto tlacitko B"
                              NL)));
            if (motorsState_ == nxpbc::MotorsState::Stay) {
                motorsState_ = nxpbc::MotorsState::Start;
            } else if (motorsState_ == nxpbc::MotorsState::Ride) {
                motorsState_ = nxpbc::MotorsState::Stay;
            }
        }
        btns_ = buttons;
    }
}
