//
// Created by zvone on 06-Apr-19.
//

#ifndef NXPCUPRASPI_IMUDATA_H
#define NXPCUPRASPI_IMUDATA_H

#include "Vec3.h"

namespace nxpbc {
  /**
   * @brief Struktura pro uložení dat z IMU jednotky pro celá čísla
   */
  union ImuData {
    struct {
      /**
       * @brief Data z gyroskopu
       */
      Vec3 gyroData = {0};
      
      /**
       * @brief Data z magnetometru
       */
      Vec3 magData = {0};
      
      /**
       * @brief Data z akcelerometru
       */
      Vec3 accelData = {0};
      
      /**
       * @brief Naměřený úhel
       */
      double angle = 0.0;
    };
    
    
  };
  
  
  /**
   * @brief Struktura pro uložení dat z IMU jednotky pro desetinná čísla
   */
  union ImuDataf {
    struct {
      /**
       * @brief Data z gyroskopu
       */
      Vec3f gyroData = {0};
      
      /**
       * @brief Data z magnetometru
       */
      Vec3f magData = {0};
      
      /**
       * @brief Data z akcelerometru
       */
      Vec3f accelData = {0};
      
      /**
       * @brief Naměřený úhel
       */
      double angle = 0.0;
    };
    
    
  };
}

#endif //NXPCUPRASPI_IMUDATA_H
