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
			tracer_{new LineTracer(TRACER_HISTORY_SIZE)},
			pid_{new PID(CONST_ERROR, CONST_INTEGRAL, CONST_DERIVATIVE, 1000.f)},
			steerRegulatorInput_{0.},
			steerRegulatorOutput_{0.},
			steerRegulatorTarget_{0.},
			steerRegulator_{new PID_new(&steerRegulatorInput_, &steerRegulatorOutput_,&steerRegulatorTarget_, CONST_ERROR, CONST_INTEGRAL, CONST_DERIVATIVE, P_ON_E, DIRECT)},
			tfc_{new TFC()},
			inSpeedCheckZone_{false},
			speedCheckZoneDebounce_{0},
			prevZonesFoundCount_{0} {

        steerRegulator_->SetOutputLimits(-1000., 1000.);
        steerRegulator_->SetSampleTime(10);

        tfc_->setPWMMax(static_cast<uint32_t>(CONTROL_DRIVE_MAX));
        tfc_->setServoCalibration(0b00, static_cast<uint32_t>(SERVO_CENTER),
                                  static_cast<uint32_t>(SERVO_LR));
        tfc_->setServoCalibration(0b01, static_cast<uint32_t>(SERVO_CENTER),
                                  static_cast<uint32_t>(SERVO_LR));
    }

    bool NxpCarAbstract::isRunning() {
        return running_;
    }

    void NxpCarAbstract::printCurrentState() {
        std::string state;
        switch (motorsState_) {
            case nxpbc::MotorsState::Stay: state = FRED("nxpbc::MotorsState::Stay");
                break;
            case nxpbc::MotorsState::Start: state = FYEL("nxpbc::MotorsState::Start");
                break;
            case nxpbc::MotorsState::Ride: state = FGRN("nxpbc::MotorsState::Ride");
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

        leftSpeed = static_cast<uint16_t>(motorSpeed_ * tfc_->ReadPot_f(0)
                                          * leftTurningCoeficient);
        rightSpeed = static_cast<uint16_t>(motorSpeed_ * tfc_->ReadPot_f(0)
                                           * rightTurningCoeficient);

        if (!(tracer_->unchangedRight_ && tracer_->unchangedLeft_)) { /*V zatacce*/
            /*Pomalejsi prujezd*/
            leftSpeed *= 0.75f;
            rightSpeed *= 0.75f;
            /*Ostrejsi rizeni*/
            servoPos_ *= 1.2f;
        }

        /*Pokud mas zpomalit, zpomal na polovinu*/
        if (tfc_->getDIPSwitch() & 0b00000010) {
            if (inSpeedCheckZone_) {
                leftSpeed *= 0.5f;
                rightSpeed *= 0.5f;
            }
        }

        tfc_->setServo_i(servoChannel_, servoPos_);
        tfc_->setMotorPWM_i(static_cast<int>(leftSpeed),
                            static_cast<int>(rightSpeed));

        sendData_.leftPwm = leftSpeed;
        sendData_.rightPwm = rightSpeed;
        sendData_.servo = servoPos_;
        sendData_.motorSpeed = motorSpeed_;

    }

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

/*void NxpCarAbstract::handleBtns(unsigned char buttons) {
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
 }*/
}
