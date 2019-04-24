//
// Created by zvone on 21-Feb-19.
//



#if defined(__linux__) || defined(WIN32)

#include "pch.h"
#include "NxpCarRaspi.h"
#include "Settings.h"
#include "motorsState.h"
#include "LineTracer.h"
#include "Logger.h"
#include "PID.h"
#include "NxpCommunication.h"
#include "Region.h"
#include "NxpImage.h"
#include "NxpModes.h"

namespace nxpbc {


    NxpCar::NxpCar() :
            setting_{nullptr},
            control_{nullptr},
            data_{nullptr},
            communication_{new NxpCommunication()} {

        communication_->openCom();

        if (!communication_->isComOpen()) {
            NXP_ERRORP("Nelze otevrit seriovy port: %s"
            NL, strerror(errno));
            running_ = false;
            return;
        }

        communication_->flushCom();
        setting_ = tfc_->getSetting();

        communication_->setSetting(setting_);
        communication_->sendComSetting();

        originalImageFile_ = new std::ofstream("recordings/" + time + "/01original.bin",
                                               std::ofstream::trunc | std::ofstream::out | std::ofstream::binary);
        normalizedImageFile_ = new std::ofstream("recordings/" + time + "/02normalized.bin",
                                                 std::ofstream::trunc | std::ofstream::out | std::ofstream::binary);
        thresholdedImageFile_ = new std::ofstream("recordings/" + time + "/03thresholded.bin",
                                                  std::ofstream::trunc | std::ofstream::out | std::ofstream::binary);
        csvFile_ = new std::ofstream("recordings/" + time + "values.csv",
                                     std::ofstream::trunc | std::ofstream::out);

        (*csvFile_)
                << "state,voltage,timestampDiff,levaVzdalenost,pravaVzdalenost,levyPomer,pravyPomer,rozdilPomeru,servo"
        NL;
#if NXP_RASPI_UDP
        printf("Cekam na UDP spojeni..."
        NL);
        communication_->receiveUdpData();
#endif

    }

    void NxpCar::writeToCsvFile(
            std::ofstream &csvFile,
            nxpbc::MotorsState state,
            float voltage,
            int timestampDiff,
            int left,
            int right,
            float leftRatio,
            float rightRatio,
            float ratioDiff,
            int servoPos) {
        csvFile << static_cast<int>(state);
        csvFile << ',';
        csvFile << voltage;
        csvFile << ',';
        csvFile << timestampDiff;
        csvFile << ',';
        csvFile << left;
        csvFile << ',';
        csvFile << right;
        csvFile << ',';
        csvFile << leftRatio;
        csvFile << ',';
        csvFile << rightRatio;
        csvFile << ',';
        csvFile << ratioDiff;
        csvFile << ',';
        csvFile << servoPos;
        csvFile << '\n';
    }

    void NxpCar::printTimestampDiff(unsigned long diff) {
        if (timestampDiff_ >= 5) {
            NXP_ERRORP("Timestamp diff: %lu"
            NL, timestampDiff_);
        } else if (timestampDiff_ >= 2) {
            NXP_WARNP("Timestamp diff: %lu"
            NL, timestampDiff_);
        } else {
            NXP_INFOP("Timestamp diff: %lu"
            NL, timestampDiff_);
        }
    }

    void NxpCar::update() {
        printf(BOLD(FCYN("------------------"
        NL)));
        printCurrentState();
        control_ = tfc_->getControl();
        communication_->setControl(control_);
        communication_->sendComControl();
        if (!communication_->receiveComData()) {
            communication_->resetCom();
        }

        data_ = communication_->getSData();

        tfc_->setData(data_);
        float voltage = tfc_->ReadBatteryVoltage_f();
        NXP_TRACEP("Voltage: %f"
        NL, voltage);

        timestampDiff_ = data_->timestamp - timestamp_;
        timestamp_ = data_->timestamp;

        printTimestampDiff(timestampDiff_);

        if (!debounce_) {
            handleBtns(data_->push_sw);
        } else {
            if (++debounceCounter_ >= debounceCounterMax_) {
                debounceCounter_ = 0;
                debounce_ = false;
            }
        }

        tfc_->getImage(0b00, image_, CAMERA_LINE_LENGTH);




        if (tfc_->getDIPSwitch() == modeSpeedZone) {
            tracer_->addImage(NxpImage(image_), true);
            tfc_->setLED(1, 1);
        } else {
            tracer_->addImage(NxpImage(image_), false);
            tfc_->setLED(1, 0);
        }

        left_ = tracer_->getLeft();
        right_ = tracer_->getRight();

        const int leftDistance = left_;
        const int rightDistance = CAMERA_LINE_LENGTH - right_;

        const float leftRatio = static_cast<float>(leftDistance) / static_cast<float>(rightDistance);
        const float rightRatio = static_cast<float>(rightDistance) / static_cast<float>(leftDistance);
        float ratioDiff = leftRatio - rightRatio;

        NxpCarAbstract::clipRatio(ratioDiff);

        if (motorsState_ != nxpbc::MotorsState::Stay) {
            pid_->debugAdcValue = (tfc_->ReadPot_f(1) + 1.f) / 2.f;
            NXP_TRACEP("pConst: %f"
            NL, pid_->pConst_ * pid_->debugAdcValue);
            steerSetting_ = pid_->getPid(0.f, ratioDiff);
        } else {
            steerSetting_ = 0.f;
        }

        switch (motorsState_) {
            case nxpbc::MotorsState::Stay:leftSpeed_ = rightSpeed_ = 0;
                tfc_->MotorPWMOnOff(0b00);
                tfc_->ServoOnOff(0b00);
                break;
            case nxpbc::MotorsState::Start:start();
                tfc_->MotorPWMOnOff(0b11);
                tfc_->ServoOnOff(0b11);;

                steer(steerSetting_);
                break;
            case nxpbc::MotorsState::Ride:
                //tfc_->setPWMMax(250);
                tfc_->MotorPWMOnOff(0b11);
                tfc_->ServoOnOff(0b11);

                steer(steerSetting_);
                break;
        }
        setRide();


        NXP_TRACEP("Servo pos: %d"
        NL, control_->servo_pos[0]);
        if (SAVE_CSV)
            if (motorsState_ != nxpbc::MotorsState::Stay)
                writeToCsvFile(*csvFile_, motorsState_, voltage, timestampDiff_, leftDistance, rightDistance, leftRatio,
                               rightRatio, ratioDiff, control_->servo_pos[0]);

#if NXP_RASPI_UDP
        if (communication_->sendUdpData()) {
            NXP_WARN("UDP serial data neodeslany"
            NL);
        } else {
            NXP_INFO("UDP serial data OK"
            NL);
        }
#endif
    }

    NxpCar::~NxpCar() {

        DELETE_AND_PARK(setting_);
        DELETE_AND_PARK(control_);
        DELETE_AND_PARK(data_);

        DELETE_AND_PARK(communication_);

        originalImageFile_->close();
        normalizedImageFile_->close();
        thresholdedImageFile_->close();
        csvFile_->close();

        DELETE_AND_PARK(originalImageFile_);
        DELETE_AND_PARK(normalizedImageFile_);
        DELETE_AND_PARK(thresholdedImageFile_);
        DELETE_AND_PARK(csvFile_);
    }

};

#endif