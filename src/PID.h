//
// Created by Richard Zvonek on 30-Nov-18.
//

#ifndef NXPCUPRASPI_PID_H
#define NXPCUPRASPI_PID_H

class Timer;

namespace nxpbc {
  /**
   * @brief Jednoduchá implementace PID regulátoru
   */
  class PID {
  
  public:
    
    PID(double pConst, double iConst, double dConst, double windupMax = 0.);
    
    double getPid(double requiredVal, double currentVal);
    
    void resetPid();
    
    //private:
    double antiWindup();
    
    Timer *timer_;
    
    double errSum_;
    double errPrev_;
    double errCurrent_;
    double prevVal_;
    double windupMax_;
    double iTerm_;
    bool antiWindup_;
    
    double debugAdcValue = 1.0;
    
    const double pConst_;
    const double iConst_;
    const double dConst_;
    
  };
}
#endif //NXPCUPRASPI_PID_H
