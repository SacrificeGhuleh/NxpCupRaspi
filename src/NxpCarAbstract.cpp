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
#include "NxpModes.h"


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
            pid_{new PID(CONST_ERROR, CONST_INTEGRAL, CONST_DERIVATIVE, 1000.f)},
            steerRegulatorInput_{0.},
            steerRegulatorOutput_{0.},
            steerRegulatorTarget_{0.},
            modeSetting_{constRide1Setting},
            tracer_{new LineTracer(TRACER_HISTORY_SIZE)},
            steerRegulator_{
                    new PID_new(&steerRegulatorInput_,
                                &steerRegulatorOutput_, &steerRegulatorTarget_,
                                modeSetting_.regulatorP_,
                                modeSetting_.regulatorI_,
                                modeSetting_.regulatorD_,
                                P_ON_E, DIRECT)},
            tfc_{new TFC()},
            inSpeedCheckZone_{false},
            speedCheckZoneDebounce_{0},
            prevZonesFoundCount_{0},
            startTimer_{START_STEPS},
            servoIncreasingBits_{0b00},
            prevServoPos_{0},
            shouldReset_{false} {

        steerRegulator_->SetOutputLimits(-1000., 1000.);
        steerRegulator_->SetSampleTime(10);


        tfc_->setPWMMax(static_cast<uint32_t>(CONTROL_PWM_MAX));
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

        if (servoPos_ > 1000)
            servoPos_ = 1000;
        if (servoPos_ < -1000)
            servoPos_ = -1000;

        //float leftTurningCoeficient = 1 + (servoPos_ / 4000.f);
        //float rightTurningCoeficient = 1 - (servoPos_ / 4000.f);

        float innerSpeed = 0.f, outerSpeed = 0.f;

        float baseSpeed = motorSpeed_ /* tfc_->ReadPot_f(0)*/;

        int16_t leftSpeed = motorSpeed_, rightSpeed = motorSpeed_;

        float turningAngle = servoToAngle();


        /*
         * Prevod ze stupnu na radiany
         * */
        turningAngle *= PI;
        turningAngle /= 180.f;


        innerSpeed = baseSpeed *
                     (1.f - modeSetting_.diffCoef_ * (ALAMAK_TRACK * tanf(turningAngle)) / 2.f * ALAMAK_WHEELBASE);
        outerSpeed = baseSpeed *
                     (1.f + modeSetting_.diffCoef_ * (ALAMAK_TRACK * tanf(turningAngle)) / 2.f * ALAMAK_WHEELBASE);


        if (!(tracer_->unchangedRight_ && tracer_->unchangedLeft_)) { /*V zatacce*/
            if (turningAngle > 0.f) {
                /*
                 * Zatacim vpravo
                 * */
                rightSpeed = innerSpeed;
                leftSpeed = outerSpeed;
            } else {
                /*
                 * Zatacim vlevo nebo rovne
                 * */

                leftSpeed = innerSpeed;
                rightSpeed = outerSpeed;
            }

            /*Pomalejsi prujezd*/
            leftSpeed *= 0.75f;
            rightSpeed *= 0.75f;
            /*Ostrejsi rizeni*/
            servoPos_ *= 1.5f;
        } else {

        }

        if (tfc_->getDIPSwitch() == modeSpeedZone) {
            /*Pokud mas zpomalit, zpomal na polovinu*/
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
        if (startTimer_ == 0) {
            motorsState_ = nxpbc::MotorsState::Ride;
            return;
        }
        --startTimer_;
        if (motorSpeed_ < modeSetting_.maxSpeed_/*CONTROL_DRIVE_MAX*/)
            motorSpeed_ += CONTROL_PWM_STEP;
    }

    TFC *NxpCarAbstract::getTfc() {
        return tfc_;
    }

    NxpCarAbstract::~NxpCarAbstract() {
        DELETE_AND_PARK(tfc_);
        DELETE_AND_PARK(tracer_);
        DELETE_AND_PARK(pid_);
        DELETE_AND_PARK(steerRegulator_);
    }

    float NxpCarAbstract::servoToAngle() {
        return servoPos_ * SERVO_TO_DEG_CONST / 200;
    }

    bool NxpCarAbstract::shouldReset() {
        return shouldReset_;
    }

    void NxpCarAbstract::resetRegulator() {
        delete steerRegulator_;
        steerRegulatorInput_ = 0.;
        steerRegulatorOutput_ = 0.;
        steerRegulatorTarget_ = 0.;

        steerRegulator_ = new PID_new(&steerRegulatorInput_,
                                      &steerRegulatorOutput_, &steerRegulatorTarget_,
                                      modeSetting_.regulatorP_,
                                      modeSetting_.regulatorI_,
                                      modeSetting_.regulatorD_,
                                      P_ON_E, DIRECT);

    }

    void NxpCarAbstract::handleReset() {
        tracer_->reset();
        resetRegulator();
    }

}
