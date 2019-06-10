//
// Created by zvone on 15-Mar-19.
//

#ifndef NXPCUPRASPI_NXPSENDFRAME_H
#define NXPCUPRASPI_NXPSENDFRAME_H

#include <cstdint>
#include "NxpDefines.h"
#include "Region.h"
#include "NxpModeSetting.h"

#if defined(__linux__)
#include <tfc.h>
#endif

#define SEND_REGIONS_NUM 15

namespace nxpbc {
    /**
     * @brief Struktura pro posílání dat přes UDP
     */
    struct SendData {
        uint16_t image[CAMERA_LINE_LENGTH] = {0};
        uint16_t adc[anLast];
        //float voltage = 0.0f;
        int16_t leftPwm = 0;
        int16_t rightPwm = 0;
        int16_t motorSpeed = 0;
        int16_t servo = 0;
        
        float error = 0.0f;

        uint8_t whiteRegionsCount = 0;
        uint8_t blackRegionsCount = 0;
        Region regions[SEND_REGIONS_NUM] = {Region()};
        Region biggestRegion = Region();
        uint8_t bits = 0x00;
        /*
         * 0b0000 0001 computed region
         * 0b0000 0010 turning
         * */
        uint8_t regionAverage[2] = {0};
        uint8_t regionMedian[2] = {0};
        int16_t imageDiversity = 0;

        float accelValues[3];
        float gyroValues[3];
        float magnetoValues[3];
        float angle;

        uint8_t currentState = 0;

        NxpModeSetting modeSetting_ = constRide1Setting;

        //bool computedRegion = false;

    };
    //union
}

#endif //NXPCUPRASPI_NXPSENDFRAME_H
