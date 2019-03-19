//
// Created by zvone on 15-Mar-19.
//

#ifndef NXPCUPRASPI_NXPSENDFRAME_H
#define NXPCUPRASPI_NXPSENDFRAME_H

#include <cstdint>
#include "NxpDefines.h"
#include "Region.h"

#if defined(__linux__)
#include <tfc.h>
#endif

#define SEND_REGIONS_NUM 15

namespace nxpbc {
    struct SendData {
        uint16_t image[CAMERA_LINE_LENGTH] = {0};
        uint16_t adc[anLast];
        float voltage = 0.0f;
        int16_t leftPwm = 0;
        int16_t rightPwm = 0;
        int16_t servo = 0;

        Region regions[SEND_REGIONS_NUM] = {Region()};
        Region biggestRegion = Region();

    };
    //union
}

#endif //NXPCUPRASPI_NXPSENDFRAME_H
