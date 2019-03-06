//
// Created by zvone on 21-Feb-19.
//

#if defined(__MCUXPRESSO)
namespace nxpbc {

    NxpCar::NxpCar() : NxpCarAbstract() {}

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

        tfc_->getImage(0b00, image_, CAMERA_LINE_LENGTH);

        tracer_->addImage(NxpImage(image_));

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
            pid_->debugAdcValue = (tfc_->ReadPot_f(1) + 1.f) / 2.f;
            NXP_TRACEP("pConst: %f"
                               NL, pid_->pConst_ * pid_->debugAdcValue);
            steerSetting_ = pid_->getPid(0.f, ratioDiff);
        } else {
            steerSetting_ = 0.f;
        }

        switch (motorsState_) {
            case MotorsState::Stay:
                leftSpeed_ = rightSpeed_ = 0;
                tfc_->MotorPWMOnOff(0b00);
                tfc_->ServoOnOff(0b00);
                break;
            case MotorsState::Start:
                start();
                tfc_->MotorPWMOnOff(0b11);
                tfc_->ServoOnOff(0b11);;

                steer(steerSetting_);
                break;
            case MotorsState::Ride:
                tfc_->setPWMMax(250);
                tfc_->MotorPWMOnOff(0b11);
                tfc_->ServoOnOff(0b11);

                steer(steerSetting_);
                break;
        }
        setRide();
    }
};
#endif
