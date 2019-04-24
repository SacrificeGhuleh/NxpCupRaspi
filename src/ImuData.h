//
// Created by zvone on 06-Apr-19.
//

#ifndef NXPCUPRASPI_IMUDATA_H
#define NXPCUPRASPI_IMUDATA_H

#include "Vec3.h"

namespace nxpbc {
    union ImuData {
        struct {
            Vec3 gyroData = {0};
            Vec3 magData = {0};
            Vec3 accelData = {0};
            double angle = 0.0;
        };


    };
    union ImuDataf {
        struct {
            Vec3f gyroData = {0};
            Vec3f magData = {0};
            Vec3f accelData = {0};
            double angle = 0.0;
        };


    };
}

#endif //NXPCUPRASPI_IMUDATA_H
