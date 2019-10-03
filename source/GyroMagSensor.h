/*
 * GyroMagSensor.h
 *
 *  Created on: Apr 6, 2019
 *      Author: zvone
 */

#ifndef GYROMAGSENSOR_H_
#define GYROMAGSENSOR_H_

#include <cstdint>
#include <fsl_fxos.h>
#include <ImuData.h>

namespace nxpbc {

#define MAX_ACCEL_AVG_COUNT 25U
#define GRAVITY_CONST 9.81345 //m/s^2
  
  /**
   * @brief Třída pro zpracování dat z IMU jednotky
   */
  class GyroMagSensor {
  public:
    GyroMagSensor();
    
    void BOARD_I2C_ReleaseBus(void);
    
    bool init();
    
    ImuDataf getProcessedData();
    
    bool isAccelStill();
    
    void callibrate();
  
  private:
    
    /*******************************************************************************
     * Prototypes
     ******************************************************************************/
    static void i2c_release_bus_delay(void);
    
    
    /*!
     * @brief Read all data from sensor function
     * @note Must calculate g_dataScale before use this function.
     */
    void Sensor_ReadData();
    
    /*!
     * @brief Magnetometer calibration
     *
     */
    void Magnetometer_Calibrate(void);
    
    void setMagnetometerCallibration(int16_t xOffset, int16_t yOffset, int16_t zOffset);
    
    ImuDataf getSensorData();
    
    ImuData getRawSensorData();
    
    
    /*!
     * @brief Delay function
     *
     * @param ticks Cycle clock delay
     */
    static void Delay(uint32_t ticks);
    
    /*******************************************************************************
     * Variables
     ******************************************************************************/
    
    
    volatile uint16_t SampleEventFlag;
    fxos_handle_t g_fxosHandle;
    uint8_t g_sensor_address[4];
    uint8_t g_sensorRange;
    uint8_t g_dataScale;
    
    int16_t g_Ax_Raw;
    int16_t g_Ay_Raw;
    int16_t g_Az_Raw;
    
    double g_Ax;
    double g_Ay;
    double g_Az;
    
    int16_t g_Ax_buff[MAX_ACCEL_AVG_COUNT];
    int16_t g_Ay_buff[MAX_ACCEL_AVG_COUNT];
    int16_t g_Az_buff[MAX_ACCEL_AVG_COUNT];
    
    int16_t g_Mx_Raw;
    int16_t g_My_Raw;
    int16_t g_Mz_Raw;
    
    int16_t g_Mx_Offset;
    int16_t g_My_Offset;
    int16_t g_Mz_Offset;
    
    double g_Mx;
    double g_My;
    double g_Mz;
    
    double g_Mx_LP;
    double g_My_LP;
    double g_Mz_LP;
    
    double g_Yaw;
    double g_Yaw_LP;
    double g_Pitch;
    double g_Roll;
    
    int16_t g_Gx_Raw;
    int16_t g_Gy_Raw;
    
    double g_Gx;
    double g_Gy;
    
    int16_t g_Gx_buff[MAX_ACCEL_AVG_COUNT];
    int16_t g_Gy_buff[MAX_ACCEL_AVG_COUNT];
    
    bool g_FirstRun;
    
    bool isInit;
    
    float xOffset;
    float yOffset;
    float zOffset;
    float xPass;
    float yPass;
    float zPass;
    
    Vec3f prevAccel;
    unsigned long unchangedAccelTimer;
  };
  
} /* namespace nxpbc */

#endif /* GYROMAGSENSOR_H_ */
