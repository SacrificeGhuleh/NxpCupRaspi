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
#include "NxpModes.h"
//#include "GyroMagSensor.h"

//#include "enet.h"



namespace nxpbc {
#define BRAKING_FORCE 0//1
#define BRAKE 0//-300
#define MAX_SERVO_CHANGE 2000

    NxpCar::NxpCar() :
            NxpCarAbstract(),
            maxCrossTimer_{LINE_CROSS_TIMER},
            lineCrossBits_{0},
            leftLineCrossTimer_{0},
            middleLineCrossTimer_{0},
            rightLineCrossTimer_{0},
            hadLeftInPrevFrame_{true},
            hadRightInPrevFrame_{true},
            leftLineLostTimer_{0},
            rightLineLostTimer_{0} {

    }

    NxpCar::~NxpCar() {

    }

    void NxpCar::update() {
        //printf(BOLD(FCYN("------------------" NL)));
        printCurrentState();
        if (motorsState_ == nxpbc::MotorsState::Start) {
            tfc_->setLEDs(0b1001);
        } else {
            tfc_->setLEDs(0b0000);
        }

        float voltage = tfc_->ReadBatteryVoltage_f();
        NXP_TRACEP("Voltage: %f"
                           NL, voltage);

        handleBtns(0U | (tfc_->getPushButton(1) << 1U) | (tfc_->getPushButton(0)));


        tfc_->getImage(0b00, sendData_.image, CAMERA_LINE_LENGTH);
        NxpImage nxpImage(sendData_.image);
        sendData_.imageDiversity = nxpImage.getDiversity_();

        switch ((NxpModes) tfc_->getDIPSwitch()) {
            case modeSpeedZone: {
                /*handle*/


                if (modeSetting_ != constSpeedZoneSetting) {
                    modeSetting_ = constSpeedZoneSetting;
                    handleReset();
                }

                tfc_->setLEDs(0b0000);

                tracer_->addImage(nxpImage/*, true*/);

                if (motorsState_ != MotorsState::Ride)
                    break;

                Region foundRegion = tracer_->getLastRawRegion();
                auto regions = tracer_->getRegions(nxpImage, 10, CAMERA_LINE_LENGTH - 10, false);

                uint8_t blackRegionsCount = 0, whiteRegionsCount = 0;

                for (Region &r : regions) {
                    if (r.isBlack()) {
                        blackRegionsCount++;
                    } else {
                        whiteRegionsCount++;
                    }
                }
                tfc_->setLED(0, 1);
                //tfc_->setLED(4, inSpeedCheckZone_);

                if (whiteRegionsCount >= 4) {
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

                if (inSpeedCheckZone_) {
                    tfc_->setLEDs(0b1111);
                }
                break;
            }
            case modeFigEight: {
                handleIrSensors();

                /*handle*/
                if (modeSetting_ != constFigure8Setting) {
                    modeSetting_ = constFigure8Setting;
                    handleReset();
                }

                tracer_->addImage(nxpImage, false);
                tfc_->setLEDs(0b0000);
                break;
            }
            case modeObstacle: {
                handleIrSensors();


                /*handle*/
                if (modeSetting_ != constObstacleSetting) {
                    modeSetting_ = constObstacleSetting;
                    handleReset();
                }

                tracer_->addImage(nxpImage, false);
                tfc_->setLEDs(0b0000);
                break;
            }
            case modeRideOne: {
                handleIrSensors();

                if (modeSetting_ != constRide1Setting) {
                    modeSetting_ = constRide1Setting;
                    handleReset();
                }

                tracer_->addImage(nxpImage, false);
                tfc_->setLEDs(0b0000);
                break;
            }
            case modeRideTwo: {
                handleIrSensors();

                if (modeSetting_ != constRide2Setting) {
                    modeSetting_ = constRide2Setting;
                    handleReset();
                }

                tracer_->addImage(nxpImage, false);
                tfc_->setLEDs(0b0000);
                break;
            }
            case modeRideThree: {
                handleIrSensors();

                if (modeSetting_ != constRide3Setting) {
                    modeSetting_ = constRide3Setting;
                    handleReset();
                }

                tracer_->addImage(nxpImage, false);
                tfc_->setLEDs(0b0000);
                break;
            }
            case modeRideSetting: {
                handleIrSensors();

                int regulatorConstValue = ((tfc_->ReadPot_f(1) + 1.f) * 100.f) / 2.f;
                int speedSetting = tfc_->ReadPot_i(0);

                /*Zaokrouhlit na desitky, BO potenciometr neni uplne presny
                 * at se zamezi stalemu resetovani*/
                regulatorConstValue /= 10;
                regulatorConstValue *= 10;

                speedSetting /= 10;
                speedSetting *= 10;


                /*
                 * je treba dynamicky menit v kodu,
                 * mame jen jeden potenciometr pro cely PID
                 * tzn - nastav P, potom D, potom I
                 * */
                NxpModeSetting setting(
                        regulatorConstValue,
                        CONST_INTEGRAL,
                        CONST_DERIVATIVE,
                        TURN_CONTROL_COEFICIENT,
                        speedSetting,
                        NxpModes::modeRideSetting);
                if (modeSetting_ != setting) {
                    modeSetting_ = setting;
                    handleReset();
                }

                tracer_->addImage(nxpImage, false);
                tfc_->setLEDs(0b0000);
                break;
            }
            case modeDiagBtns:
            case modeDiagServo:
            case modeDiagMotors:
            case modeDiagCam:
            default: break;
        }


        auto dst = tracer_->getDistancesPair();
        left_ = dst.first;
        right_ = dst.second;

        /*pokud prave ztratil levou caru*/
        if (tracer_->hasLeft() != hadLeftInPrevFrame_ && !tracer_->hasLeft()) {

        }
        /*pokud prave ztratil pravou caru*/
        if (tracer_->hasRight() != hadRightInPrevFrame_ && !tracer_->hasRight()) {

        }

        /*Nema ani jednu caru*/
        if (!tracer_->hasLeft() && !tracer_->hasRight()) {

            /*Nema pravou, ma levou*/
        } else if (!tracer_->hasLeft() && tracer_->hasRight()) {

            /*Nema levou, ma pravou*/
        } else if (tracer_->hasLeft() && !tracer_->hasRight()) {

        }

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
            steerRegulatorInput_ = ratioDiff;
            steerRegulator_->Compute();
            steerSetting_ = static_cast<float>(steerRegulatorOutput_);
            pid_->debugAdcValue = (45.f);
        } else {
            steerSetting_ = 0.f;
        }

        prevServoPos_ = servoPos_;

        switch (motorsState_) {
            case MotorsState::Stay: {
                startTimer_ = START_STEPS;
                motorSpeed_ = 0;
                steerRegulator_->SetMode(MANUAL);
                servoPos_ = 0;
                break;
            }
            case MotorsState::Start: {
                //tfc_->setPWMMax(modeSetting_.maxSpeed);
                tfc_->setPWMMax(CONTROL_PWM_MAX);
                start();
                steerRegulator_->SetMode(AUTOMATIC);
                servoPos_ = static_cast<int16_t>(steerSetting_);

                break;
            }
            case MotorsState::Ride: {
                //tfc_->setPWMMax(modeSetting_.maxSpeed);
                tfc_->setPWMMax(CONTROL_PWM_MAX);
                motorSpeed_ = modeSetting_.maxSpeed_;
                //tfc_->MotorPWMOnOff(0b11);
                //tfc_->ServoOnOff(0b11);

                if ((NxpModes) tfc_->getDIPSwitch() == NxpModes::modeSpeedZone)
                    if (nxpImage.isLowDiversity()) {
                        //nastaveni serva zustane

                        //servoPos_ = servoPos_;
                        break;
                    }

                servoPos_ = static_cast<int16_t>(steerSetting_);
//                steer(steerSetting_);
                break;
            }
            case MotorsState::Brake: {
                tfc_->setPWMMax(CONTROL_PWM_MAX);
                motorSpeed_ = MIN((motorSpeed_ + BRAKING_FORCE), 0);
                servoPos_ = static_cast<int16_t>(steerSetting_);
                break;
            }
        }


        int16_t servoChange = abs(MAX(servoPos_, prevServoPos_) - MIN(servoPos_, prevServoPos_));
        if (servoPos_ > prevServoPos_) {
            //natoceni je VETSI
            servoIncreasingBits_ = 0b01;

            if (servoChange > MAX_SERVO_CHANGE) {
                servoPos_ = prevServoPos_ + MAX_SERVO_CHANGE;
            }

        } else if (servoPos_ < prevServoPos_) {
            //natoceni je mensi
            servoIncreasingBits_ = 0b10;

            if (servoChange > MAX_SERVO_CHANGE) {
                servoPos_ = prevServoPos_ - MAX_SERVO_CHANGE;
            }
        } else {
            servoChange = 0;
            servoIncreasingBits_ = 0b00;
        }

        setSendData();

        setRide();
    }

    void NxpCar::handleIrSensors() {
        if (this->motorsState_ == MotorsState::Ride) {
            if (this->tfc_->ReadADC(anIR_1) > WHITE_IR_BOUND) {
                bitWrite(this->lineCrossBits_, 0, 1);
                this->leftLineCrossTimer_ = this->maxCrossTimer_;
            } else if (this->leftLineCrossTimer_ > 0) {
                this->leftLineCrossTimer_--;
            } else if (this->leftLineCrossTimer_ == 0) {
                bitWrite(this->lineCrossBits_, 0, 0);
            }

            if (this->tfc_->ReadADC(anIR_2) > WHITE_IR_BOUND) {
                bitWrite(this->lineCrossBits_, 1, 1);
                this->middleLineCrossTimer_ = this->maxCrossTimer_;
            } else if (this->middleLineCrossTimer_ > 0) {
                this->middleLineCrossTimer_--;
            } else if (this->middleLineCrossTimer_ == 0) {
                bitWrite(this->lineCrossBits_, 1, 0);
            }

            if (this->tfc_->ReadADC(anIR_3) > WHITE_IR_BOUND) {
                bitWrite(this->lineCrossBits_, 2, 1);
                this->rightLineCrossTimer_ = this->maxCrossTimer_;
            } else if (this->rightLineCrossTimer_ > 0) {
                this->rightLineCrossTimer_--;
            } else if (this->rightLineCrossTimer_ == 0) {
                bitWrite(this->lineCrossBits_, 2, 0);
            }

            if (this->lineCrossBits_ == 0b111 || this->lineCrossBits_ == 0b101) {
                motorsState_ = MotorsState::Brake;
                motorSpeed_ = BRAKE;
                this->lineCrossBits_ = 0b000;
                this->leftLineCrossTimer_ = 0;
                this->middleLineCrossTimer_ = 0;
                this->rightLineCrossTimer_ = 0;
            }
        }
    }

    void NxpCar::handleBtns(unsigned char buttons) {
        if (!debounce_) {
            debounce_ = true;
            /*Tlacitko A*/
            if ((buttons & 0b01) && ((buttons & 0b01) != (btns_ & 0b01))) {
                NXP_WARN(BOLD(FRED("Resetuji rozpoznavani car"
                                 NL)));
                tracer_->reset();
                shouldReset_ = true;
                inSpeedCheckZone_ = false;
            }

            /*Tlacitko B*/
            if ((buttons & 0b10) && ((buttons & 0b10) != (btns_ & 0b10))) {
                /*Reset regulatoru*/
                resetRegulator();
                /*Konec resetu*/
                if (motorsState_ == nxpbc::MotorsState::Stay) {
                    motorsState_ = nxpbc::MotorsState::Start;
                } else if (motorsState_ == nxpbc::MotorsState::Ride || motorsState_ == nxpbc::MotorsState::Brake) {
                    motorsState_ = nxpbc::MotorsState::Stay;
                }
            }
            inSpeedCheckZone_ = false;
            btns_ = buttons;

        } else {
            if (++debounceCounter_ >= debounceCounterMax_) {
                debounceCounter_ = 0;
                debounce_ = false;
            }
        }
    }

    void NxpCar::setSendData() {

        for (uint8_t i = 0; i < anLast; i++) {
            sendData_.adc[i] = tfc_->ReadADC(i);
        }

        for (uint8_t i = 0; i < SEND_REGIONS_NUM; i++) {
            sendData_.regions[i] = Region();
        }


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

        sendData_.currentState = static_cast<uint8_t>(motorsState_);

        sendData_.modeSetting_ = modeSetting_;

    }
};
#endif
