//
// Created by zvone on 21-Feb-19.
//

#if defined(__MCUXPRESSO) || 1

#include "pch.h"
#include "NxpCarFreescale.h"
#include "NxpDefines.h"
#include "Logger.h"
#include "LineTracer.h"
#include "motorsState.h"
#include "PID.h"
#include "PID_New.h"
#include "Settings.h"

#include "enet.h"

namespace nxpbc {

#define DIVERSITY_COEFICIENT 100

    NxpCar::NxpCar() :
            NxpCarAbstract() {
    }

    NxpCar::~NxpCar() {

    }

    void NxpCar::update() {
        printf(BOLD(FCYN("------------------"
                            NL)));
        printCurrentState();
        float voltage = tfc_->ReadBatteryVoltage_f();
        NXP_TRACEP("Voltage: %f"
                           NL, voltage);

        if (!debounce_) {
            handleBtns(0 | (tfc_->getPushButton(1) << 1) | (tfc_->getPushButton(0)));
        } else {
            if (++debounceCounter_ >= debounceCounterMax_) {
                debounceCounter_ = 0;
                debounce_ = false;
            }
        }

        tfc_->getImage(0b00, sendData_.image, CAMERA_LINE_LENGTH);

        NxpImage nxpImage(sendData_.image);

        sendData_.imageDiversity = nxpImage.getDiversity_();

        if (tfc_->getDIPSwitch() & 0b00000010) {
            tfc_->setLED(0, 0);
            tfc_->setLED(1, 0);
            tfc_->setLED(2, 0);
            tfc_->setLED(3, 0);

            //if (nxpImage.getDiversity_() < DIVERSITY_COEFICIENT && tracer_->getListSize() > 0) {
                tracer_->addImage(nxpImage, true);
            //}
            uint8_t blackRegionsCount = 0, whiteRegionsCount = 0;

            for (Region &r : tracer_->currentRegions_) {
                if (r.isBlack()) {
                    blackRegionsCount++;
                } else {
                    whiteRegionsCount++;
                }
            }
            tfc_->setLED(0, 1);

            int foundLines = tracer_->computedRegion_ ? 1 : 2;

            tfc_->setLED(4, inSpeedCheckZone_);

            if (whiteRegionsCount > 3) {
                if (prevZonesFoundCount_ == whiteRegionsCount) {
                    if (speedCheckZoneDebounce_ != 0)
                        speedCheckZoneDebounce_--;

                } else {
                    tfc_->setLED(2, 1);
                    if (speedCheckZoneDebounce_ != 0) {
                        speedCheckZoneDebounce_--;
                    } else {
                        inSpeedCheckZone_ = !inSpeedCheckZone_;
                        speedCheckZoneDebounce_ = 40;

                    }
                }
            }
            prevZonesFoundCount_ = whiteRegionsCount;
            tfc_->setLED(4, inSpeedCheckZone_);

            if (inSpeedCheckZone_) {
                tfc_->setLEDs(0xFF);
            }

        } else {
            //if (nxpImage.getDiversity_() < DIVERSITY_COEFICIENT && tracer_->getListSize() > 0) {
                tracer_->addImage(nxpImage, false);
            //}
            tfc_->setLED(0, 0);
            tfc_->setLED(1, 0);
            tfc_->setLED(2, 0);
            tfc_->setLED(3, 0);
        }

        auto dst = tracer_->getDistancesPair();
        left_ = dst.first;
        right_ = dst.second;

        //left_ = tracer_->getLeft();
        //right_ = tracer_->getRight();

        const int leftDistance = left_;
        const int rightDistance = CAMERA_LINE_LENGTH - right_;

        const float leftRatio = static_cast<float>(leftDistance)
                                / static_cast<float>(rightDistance);
        const float rightRatio = static_cast<float>(rightDistance)
                                 / static_cast<float>(leftDistance);
        //float ratioDiff = leftRatio - rightRatio;
        float ratioDiff = rightRatio - leftRatio;

        sendData_.error = ratioDiff;

        //NxpCarAbstract::clipRatio(ratioDiff);

        if (motorsState_ != MotorsState::Stay) {
            /*steerRegulatorInput_ = ratioDiff
                                   * ((tfc_->ReadPot_f(1) + 1.f) / 2.f * 100);*/
            steerRegulatorInput_ = ratioDiff;
            steerSetting_ = static_cast<float>(steerRegulatorOutput_);
            //pid_->debugAdcValue = (tfc_->ReadPot_f(1) + 1.f) /*/ 2.f*/;
            pid_->debugAdcValue = (45.f) /*/ 2.f*/;
            /*NXP_TRACEP("pConst: %f"
            NL, pid_->pConst_ * pid_->debugAdcValue);*/
            //steerSetting_ = static_cast<float>(pid_->getPid(0.f, ratioDiff));
            steerRegulator_->Compute();
        } else {
            steerSetting_ = 0.f;
        }

        switch (motorsState_) {
            case MotorsState::Stay: motorSpeed_ = 0;
                steerRegulator_->SetMode(MANUAL);
                servoPos_ = 0;
                //tfc_->MotorPWMOnOff(0b00);
                //tfc_->ServoOnOff(0b00);
                break;
            case MotorsState::Start: start();
                steerRegulator_->SetMode(AUTOMATIC);
                //tfc_->MotorPWMOnOff(0b11);
                //tfc_->ServoOnOff(0b11);
                servoPos_ = static_cast<int16_t>(steerSetting_);
//                steer(steerSetting_);
                break;
            case MotorsState::Ride: tfc_->setPWMMax(250);
                //tfc_->MotorPWMOnOff(0b11);
                //tfc_->ServoOnOff(0b11);
                servoPos_ = static_cast<int16_t>(steerSetting_);
//                steer(steerSetting_);
                break;
        }

        setSendData();

        setRide();
        //enet->send(&sendData_, sizeof(SendData));
    }

    void NxpCar::handleBtns(unsigned char buttons) {
        debounce_ = true;
        /*Tlacitko A*/
        if ((buttons & 0b01) && ((buttons & 0b01) != (btns_ & 0b01))) {
            NXP_WARN(BOLD(FRED("Resetuji rozpoznavani car"
                             NL)));
            tracer_->reset();

        }

        /*Tlacitko B*/
        if ((buttons & 0b10) && ((buttons & 0b10) != (btns_ & 0b10))) {
            NXP_TRACE(BOLD(FRED("Menim stav motoru"
                              NL)));

            /*Reset regulatoru*/
            delete steerRegulator_;
            steerRegulatorInput_ = 0.;
            steerRegulatorOutput_ = 0.;
            steerRegulatorTarget_ = 0.;

            steerRegulator_ = new PID_new(&steerRegulatorInput_,
                                          &steerRegulatorOutput_, &steerRegulatorTarget_,
                                          CONST_ERROR,
                                          CONST_INTEGRAL, CONST_DERIVATIVE, P_ON_E, DIRECT);
            /*Konec resetu*/
            if (motorsState_ == nxpbc::MotorsState::Stay) {
                motorsState_ = nxpbc::MotorsState::Start;
            } else if (motorsState_ == nxpbc::MotorsState::Ride) {
                motorsState_ = nxpbc::MotorsState::Stay;
            }
        }
        btns_ = buttons;
    }

    void NxpCar::setSendData() {

        for (uint8_t i = 0; i < anLast; i++) {
            sendData_.adc[i] = tfc_->ReadADC(i);
        }

        for (uint8_t i = 0; i < SEND_REGIONS_NUM; i++) {
            sendData_.regions[i] = Region();
        }

/*
    	sendData_.blackRegionsCount = 0;
        sendData_.whiteRegionsCount = MIN(SEND_REGIONS_NUM,
                                     tracer_->currentRegions_.size());
        for (uint8_t i = 0; i < sendData_.whiteRegionsCount; i++) {
            if (tracer_->currentRegions_.at(i).isWhite())
                sendData_.regions[i] = tracer_->currentRegions_.at(i);
            else{
            	sendData_.blackRegionsCount++;
            }
        }*/

        sendData_.blackRegionsCount = tracer_->blackRegionsCount_;
        sendData_.whiteRegionsCount = MIN(SEND_REGIONS_NUM,
                                          tracer_->whiteRegionsCount_);
        for (uint8_t i = 0; i < sendData_.whiteRegionsCount; i++) {
            if (tracer_->currentRegions_.at(i).isWhite())
                sendData_.regions[i] = tracer_->currentRegions_.at(i);
        }

        sendData_.biggestRegion = Region(left_, right_, COLOR_WHITE);
        sendData_.bits = 0x00;
        sendData_.bits |= tracer_->computedRegion_ << 0;  //computed region
        sendData_.bits |= (!(tracer_->unchangedRight_ && tracer_->unchangedLeft_)) << 1; //v zatacce

        sendData_.regionAverage[0] = tracer_->currentAverage_.first;
        sendData_.regionAverage[1] = tracer_->currentAverage_.second;

        sendData_.regionMedian[0] = tracer_->currentMedian_.first;
        sendData_.regionMedian[1] = tracer_->currentMedian_.second;

        //sendData_.imageDiversity = 0;

    }
};
#endif
