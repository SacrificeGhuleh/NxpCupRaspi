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

    NxpCar::NxpCar() : NxpCarAbstract() {
        //enet = new Enet();
        //enet->init(256, 4444);
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
            handleBtns(/*data_->push_sw*/0 | (tfc_->getPushButton(1) << 1) | (tfc_->getPushButton(0)));
        } else {
            if (++debounceCounter_ >= debounceCounterMax_) {
                debounceCounter_ = 0;
                debounce_ = false;
            }
        }

        tfc_->getImage(0b00, sendData_.image, CAMERA_LINE_LENGTH);

        tracer_->addImage(NxpImage(sendData_.image));

        left_ = tracer_->getLeft();
        right_ = tracer_->getRight();

        const int leftDistance = left_;
        const int rightDistance = CAMERA_LINE_LENGTH - right_;

        const float leftRatio = static_cast<float>(leftDistance) / static_cast<float>(rightDistance);
        const float rightRatio = static_cast<float>(rightDistance) / static_cast<float>(leftDistance);
        //float ratioDiff = leftRatio - rightRatio;
        float ratioDiff = rightRatio - leftRatio;

        //NxpCarAbstract::clipRatio(ratioDiff);

        if (motorsState_ != MotorsState::Stay) {
            steerRegulatorInput = ratioDiff * ((tfc_->ReadPot_f(1) + 1.f) / 2.f * 100);
            steerSetting_ = static_cast<float>(steerRegulatorOutput);
            pid_->debugAdcValue = (tfc_->ReadPot_f(1) + 1.f) /*/ 2.f*/;
            /*NXP_TRACEP("pConst: %f"
                               NL, pid_->pConst_ * pid_->debugAdcValue);*/
            //steerSetting_ = static_cast<float>(pid_->getPid(0.f, ratioDiff));
        } else {
            steerSetting_ = 0.f;
        }
        steerRegulator->Compute();
        switch (motorsState_) {
            case MotorsState::Stay:motorSpeed_ = 0;
                steerRegulator->SetMode(MANUAL);
                //tfc_->MotorPWMOnOff(0b00);
                //tfc_->ServoOnOff(0b00);
                break;
            case MotorsState::Start:start();
                steerRegulator->SetMode(AUTOMATIC);
                //tfc_->MotorPWMOnOff(0b11);
                //tfc_->ServoOnOff(0b11);
                servoPos_ = static_cast<int16_t>(steerSetting_);
//                steer(steerSetting_);
                break;
            case MotorsState::Ride:tfc_->setPWMMax(250);
                //tfc_->MotorPWMOnOff(0b11);
                //tfc_->ServoOnOff(0b11);
                servoPos_ = static_cast<int16_t>(steerSetting_);
//                steer(steerSetting_);
                break;
        }

        for (int i = 0; i < anLast; i++) {
            sendData_.adc[i] = tfc_->ReadADC(i);
        }

        for (int i = 0; i < SEND_REGIONS_NUM; i++) {
            sendData_.regions[i] = Region();
        }
        auto it = tracer_->imageRegionList_.begin();
        for (int i = 0; i < MIN(SEND_REGIONS_NUM, tracer_->imageRegionList_.size()); i++) {
            sendData_.regions[i] = *it;//Region(it->left, it->right, it->color);
            ++it;
        }

        sendData_.biggestRegion = Region(left_, right_, COLOR_WHITE);


        setRide();
        //enet->send(&sendData_, sizeof(SendData));
    }
};
#endif
