//
// Created by zvone on 24-Apr-19.
//

#ifndef NXPCUPRASPI_NXPMODES_H
#define NXPCUPRASPI_NXPMODES_H

namespace nxpbc{

    enum NxpModes{
        modeDiagBtns    = 0b0001,
        modeDiagServo   = 0b0011,
        modeDiagMotors  = 0b0101,
        modeDiagCam     = 0b1001,
        modeSpeedZone   = 0b0010,
        modeFigEight    = 0b0100,
        modeObstacle    = 0b1000,
        modeRideOne     = 0b0000,
        modeRideTwo     = 0b0110,
        modeRideThree   = 0b1010,
        modeRideSetting = 0b1110
    };

}

#endif //NXPCUPRASPI_NXPMODES_H
