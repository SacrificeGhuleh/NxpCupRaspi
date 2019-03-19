//
// Created by Richard Zvonek on 30-Nov-18.
//


#include "pch.h"
#include "PID.h"
#include "Logger.h"
#include "Timer.h"

namespace nxpbc {
    double PID::getPid(double requiredVal, double currentVal) {
        //double deltaTime = timer_->elapsed();
        //timer_->reset();
        //NXP_TRACEP("PID delta time: %f", deltaTime);
        errCurrent_ = requiredVal - currentVal;

        /*errPrev_ = errCurrent_;
        errCurrent_ = requiredVal = currentVal;

        errSum_ += errCurrent_;
        */

        //proporcni * error + integral + derivativni *
        //double out = kp_ * error + iTerm_ + kd_ * (last_y - y_measured)/dt;
        iTerm_ = iTerm_ + (iConst_ * errCurrent_ * 10/*deltaTime*/);

        iTerm_ = antiWindup();

        double pidOutput = (errCurrent_ * (pConst_ * debugAdcValue)) + iTerm_ +
                           dConst_ * (prevVal_ - currentVal) / 10/*deltaTime*/;
        errPrev_ = errCurrent_;
        prevVal_ = currentVal;
        return pidOutput;
        /*
        return (errCurrent_ * (pConst_ * debugAdcValue)) + (iConst_ * errSum_ / deltaTime) +
               (dConst_ * (errCurrent_ - errPrev_) / deltaTime);*/
    }

    PID::PID(double pConst, double iConst, double dConst, double windupMax) :
            pConst_{pConst},
            iConst_{iConst},
            dConst_{dConst},
            windupMax_{windupMax},
            antiWindup_{windupMax_ != 0} {
        timer_ = new Timer();
        resetPid();

        NXP_INFOP("pConst: %f"
                          NL, pConst_);
        NXP_INFOP("iConst: %f"
                          NL, iConst_);
        NXP_INFOP("dConst: %f"
                          NL, dConst_);

    }

    void PID::resetPid() {
        errCurrent_ = 0;
        errPrev_ = 0;
        errSum_ = 0;
        timer_->reset();

    }

    double PID::antiWindup() {
        if (!antiWindup_) {
            return iTerm_;
        } else if (iTerm_ >= windupMax_) {
            return windupMax_;
        } else if (iTerm_ <= -windupMax_) {
            return -windupMax_;
        } else {
            return iTerm_;
        }
    }
}
