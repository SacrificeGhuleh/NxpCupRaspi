/*
 * GyroMagSensor.cpp
 *
 *  Created on: Apr 6, 2019
 *      Author: zvone
 */

#include <GyroMagSensor.h>

#include "fsl_debug_console.h"
#include "fsl_fxos.h"
#include "board.h"
#include "math.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "GyroMagSensor.h"


namespace nxpbc {


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define ACCEL_I2C_CLK_SRC I2C0_CLK_SRC
#define ACCEL_I2C_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_BAUDRATE 100000U
#define ACCEL_I2C_ADDR 1DU

#define I2C_RELEASE_SDA_PORT PORTD
#define I2C_RELEASE_SCL_PORT PORTD
#define I2C_RELEASE_SDA_GPIO GPIOD
#define I2C_RELEASE_SDA_PIN 9U
#define I2C_RELEASE_SCL_GPIO GPIOD
#define I2C_RELEASE_SCL_PIN 8U
#define I2C_RELEASE_BUS_COUNT 100U
#define HWTIMER_PERIOD      10000U
/* multiplicative conversion constants */

/**
 * @brief Konstanta pro prevod Deg--> Rad
 */
#define DegToRad 0.017453292

/**
 * @brief Konstanta pro prevod Rad--> Deg
 */
#define RadToDeg 57.295779

/**
 * @brief Defaultni hodnota kalibrace magnetometru pro osu x
 */
#define DEFAULT_MAG_CALLIBRATION_X -785

/**
 * @brief Defaultni hodnota kalibrace magnetometru pro osu y
 */
#define DEFAULT_MAG_CALLIBRATION_Y 1004

/**
 * @brief Defaultni hodnota kalibrace magnetometru pro osu z
 */
#define DEFAULT_MAG_CALLIBRATION_Z 538


/**
 * @brief Počet vzorků pro určení stálosti zrychlení
 */
#define ACCEL_TIMER_STILL_VALUE 20
  
  GyroMagSensor::GyroMagSensor() :
      g_sensor_address{0x1CU, 0x1EU, 0x1DU, 0x1FU},
      g_sensorRange{0},
      g_dataScale{0},
      
      g_Ax_Raw{0},
      g_Ay_Raw{0},
      g_Az_Raw{0},
      
      g_Ax{0},
      g_Ay{0},
      g_Az{0},
      
      g_Ax_buff{0},
      g_Ay_buff{0},
      g_Az_buff{0},
      
      g_Mx_Raw{0},
      g_My_Raw{0},
      g_Mz_Raw{0},
      
      g_Mx_Offset{0},
      g_My_Offset{0},
      g_Mz_Offset{0},
      
      g_Mx{0},
      g_My{0},
      g_Mz{0},
      
      g_Mx_LP{0},
      g_My_LP{0},
      g_Mz_LP{0},
      
      g_Yaw{0},
      g_Yaw_LP{0},
      g_Pitch{0},
      g_Roll{0},
      
      g_Gx{0},
      g_Gy{0},
      g_Gx_Raw{0},
      g_Gy_Raw{0},
      
      g_FirstRun{true},
      isInit{false},
      
      xOffset{0},
      yOffset{0},
      zOffset{0},
      xPass{0},
      yPass{0},
      zPass{0},
      prevAccel{0, 0, 0},
      unchangedAccelTimer{0} {
    
  }
  
  void GyroMagSensor::BOARD_I2C_ReleaseBus(void) {
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};
    
    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux = kPORT_MuxAsGpio;
    
    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SDA_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);
    
    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);
    
    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();
    
    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++) {
      GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
      i2c_release_bus_delay();
      
      GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
      i2c_release_bus_delay();
      
      GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
      i2c_release_bus_delay();
      i2c_release_bus_delay();
    }
    
    /* Send stop */
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();
    
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();
    
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();
    
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
  }
  
  void GyroMagSensor::Sensor_ReadData() {
    fxos_data_t fxos_data;
    
    if (FXOS_ReadSensorData(&g_fxosHandle, &fxos_data) != kStatus_Success) {
      PRINTF("Failed to read acceleration data!\r\n");
      return;
    }
    /* Get the accel data from the sensor data structure in 14 bit left format data*/
    g_Ax_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.accelXMSB << 8) | (uint16_t) fxos_data.accelXLSB) / 4U;
    g_Ay_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.accelYMSB << 8) | (uint16_t) fxos_data.accelYLSB) / 4U;
    g_Az_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.accelZMSB << 8) | (uint16_t) fxos_data.accelZLSB) / 4U;
    
    g_Ax_Raw *= g_dataScale;
    g_Ay_Raw *= g_dataScale;
    g_Az_Raw *= g_dataScale;
    
    g_Mx_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.magXMSB << 8) | (uint16_t) fxos_data.magXLSB);
    g_My_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.magYMSB << 8) | (uint16_t) fxos_data.magYLSB);
    g_Mz_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.magZMSB << 8) | (uint16_t) fxos_data.magZLSB);
    
    g_Gx_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.accelXMSB << 8) | (uint16_t) fxos_data.accelXLSB) / 4U;
    g_Gy_Raw = (int16_t) ((uint16_t) ((uint16_t) fxos_data.accelYMSB << 8) | (uint16_t) fxos_data.accelYLSB) / 4U;
  }
  
  void GyroMagSensor::Magnetometer_Calibrate(void) {
    int16_t Mx_max = 0;
    int16_t My_max = 0;
    int16_t Mz_max = 0;
    int16_t Mx_min = 0;
    int16_t My_min = 0;
    int16_t Mz_min = 0;
    
    uint32_t times = 0;
    PRINTF("\r\nCalibrating magnetometer...");
    while (times < 5000) {
      Sensor_ReadData(/*&g_Ax_Raw, &g_Ay_Raw, &g_Az_Raw, &g_Mx_Raw, &g_My_Raw, &g_Mz_Raw*/);
      if (times == 0) {
        Mx_max = Mx_min = g_Mx_Raw;
        My_max = My_min = g_My_Raw;
        Mz_max = Mz_min = g_Mz_Raw;
      }
      if (g_Mx_Raw > Mx_max) {
        Mx_max = g_Mx_Raw;
      }
      if (g_My_Raw > My_max) {
        My_max = g_My_Raw;
      }
      if (g_Mz_Raw > Mz_max) {
        Mz_max = g_Mz_Raw;
      }
      if (g_Mx_Raw < Mx_min) {
        Mx_min = g_Mx_Raw;
      }
      if (g_My_Raw < My_min) {
        My_min = g_My_Raw;
      }
      if (g_Mz_Raw < Mz_min) {
        Mz_min = g_Mz_Raw;
      }
      if (times == 4999) {
        if ((Mx_max > (Mx_min + 500)) && (My_max > (My_min + 500)) && (Mz_max > (Mz_min + 500))) {
          g_Mx_Offset = (Mx_max + Mx_min) / 2;
          g_My_Offset = (My_max + My_min) / 2;
          g_Mz_Offset = (Mz_max + Mz_min) / 2;
          PRINTF("\r\nCalibrate magnetometer successfully!");
          PRINTF("\r\nMagnetometer offset Mx: %d - My: %d - Mz: %d \r\n", g_Mx_Offset, g_My_Offset,
                 g_Mz_Offset);
        } else {
          PRINTF("Calibrating magnetometer failed! Press any key to recalibrate...\r\n");
          GETCHAR();
          PRINTF("\r\nCalibrating magnetometer...");
          times = 0;
        }
      }
      times++;
      Delay(3000);
    }
  }
  
  
  void GyroMagSensor::i2c_release_bus_delay(void) {
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++) {
      __NOP();
    }
  }
  
  bool GyroMagSensor::init() {
    if (isInit)
      return false;
    
    
    fxos_config_t config = {0};
    uint8_t array_addr_size = 0;
    status_t result;
    
    isInit = false;
    /* Configure the I2C function */
    config.I2C_SendFunc = BOARD_Accel_I2C_Send;
    config.I2C_ReceiveFunc = BOARD_Accel_I2C_Receive;
    
    /* Initialize sensor devices */
    array_addr_size = sizeof(g_sensor_address) / sizeof(g_sensor_address[0]);
    for (int i = 0; i < array_addr_size; i++) {
      config.slaveAddress = g_sensor_address[i];
      /* Initialize accelerometer sensor */
      result = FXOS_Init(&g_fxosHandle, &config);
      if (result == kStatus_Success) {
        break;
      }
    }
    
    if (result != kStatus_Success) {
      PRINTF("\r\nSensor device initialize failed!\r\n");
      return isInit;
    }
    
    /* Get sensor range */
    if (FXOS_ReadReg(&g_fxosHandle, XYZ_DATA_CFG_REG, &g_sensorRange, 1) != kStatus_Success) {
      return isInit;
    }
    if (g_sensorRange == 0x00) {
      g_dataScale = 2U;
      isInit = true;
    } else if (g_sensorRange == 0x01) {
      g_dataScale = 4U;
      isInit = true;
    } else if (g_sensorRange == 0x10) {
      g_dataScale = 8U;
      isInit = true;
    } else {
    }
    return isInit;
  }
  
  void GyroMagSensor::Delay(uint32_t ticks) {
    while (ticks--) {
      __asm("nop");
    }
  }
  
  void GyroMagSensor::setMagnetometerCallibration(int16_t xOffset, int16_t yOffset, int16_t zOffset) {
    g_Mx_Offset = xOffset;
    g_My_Offset = yOffset;
    g_Mz_Offset = zOffset;
  }
  
  ImuDataf GyroMagSensor::getSensorData() {
    ImuDataf result = {0};
    
    double sinAngle = 0;
    double cosAngle = 0;
    double Bx = 0;
    double By = 0;
    
    SampleEventFlag = 0;
    
    g_Ax_Raw = 0;
    g_Ay_Raw = 0;
    g_Az_Raw = 0;
    
    g_Ax = 0;
    g_Ay = 0;
    g_Az = 0;
    
    g_Mx_Raw = 0;
    g_My_Raw = 0;
    g_Mz_Raw = 0;
    
    g_Mx = 0;
    g_My = 0;
    g_Mz = 0;
    
    
    g_Gx_Raw = 0;
    g_Gy_Raw = 0;
    
    g_Gx = 0;
    g_Gy = 0;
    
    /* Read sensor data */
    Sensor_ReadData();
    
    /* Average accel value */
    for (int i = 1; i < MAX_ACCEL_AVG_COUNT; i++) {
      g_Ax_buff[i] = g_Ax_buff[i - 1];
      g_Ay_buff[i] = g_Ay_buff[i - 1];
      g_Az_buff[i] = g_Az_buff[i - 1];
      
      g_Gx_buff[i] = g_Gx_buff[i - 1];
      g_Gy_buff[i] = g_Gy_buff[i - 1];
    }
    
    g_Ax_buff[0] = g_Ax_Raw;
    g_Ay_buff[0] = g_Ay_Raw;
    g_Az_buff[0] = g_Az_Raw;
    
    g_Gx_buff[0] = g_Gx_Raw;
    g_Gy_buff[0] = g_Gx_Raw;
    
    for (int i = 0; i < MAX_ACCEL_AVG_COUNT; i++) {
      g_Ax += (double) g_Ax_buff[i];
      g_Ay += (double) g_Ay_buff[i];
      g_Az += (double) g_Az_buff[i];
      
      g_Gx += g_Gx_buff[i];
      g_Gy += g_Gy_buff[i];
    }
    
    g_Ax /= MAX_ACCEL_AVG_COUNT;
    g_Ay /= MAX_ACCEL_AVG_COUNT;
    g_Az /= MAX_ACCEL_AVG_COUNT;
    
    g_Gx /= MAX_ACCEL_AVG_COUNT;
    g_Gy /= MAX_ACCEL_AVG_COUNT;
    
    if (g_FirstRun) {
      g_Mx_LP = g_Mx_Raw;
      g_My_LP = g_My_Raw;
      g_Mz_LP = g_Mz_Raw;
    }
    
    g_Mx_LP += ((double) g_Mx_Raw - g_Mx_LP) * 0.01;
    g_My_LP += ((double) g_My_Raw - g_My_LP) * 0.01;
    g_Mz_LP += ((double) g_Mz_Raw - g_Mz_LP) * 0.01;
    
    /* Calculate magnetometer values */
    g_Mx = g_Mx_LP - g_Mx_Offset;
    g_My = g_My_LP - g_My_Offset;
    g_Mz = g_Mz_LP - g_Mz_Offset;
    
    /* Calculate roll angle g_Roll (-180deg, 180deg) and sin, cos */
    g_Roll = atan2(g_Ay, g_Az) * RadToDeg;
    sinAngle = sin(g_Roll * DegToRad);
    cosAngle = cos(g_Roll * DegToRad);
    
    /* De-rotate by roll angle g_Roll */
    By = g_My * cosAngle - g_Mz * sinAngle;
    g_Mz = g_Mz * cosAngle + g_My * sinAngle;
    g_Az = g_Ay * sinAngle + g_Az * cosAngle;
    
    /* Calculate pitch angle g_Pitch (-90deg, 90deg) and sin, cos*/
    g_Pitch = atan2(-g_Ax, g_Az) * RadToDeg;
    sinAngle = sin(g_Pitch * DegToRad);
    cosAngle = cos(g_Pitch * DegToRad);
    
    /* De-rotate by pitch angle g_Pitch */
    Bx = g_Mx * cosAngle + g_Mz * sinAngle;
    
    /* Calculate yaw = ecompass angle psi (-180deg, 180deg) */
    g_Yaw = atan2(-By, Bx) * RadToDeg;
    if (g_FirstRun) {
      g_Yaw_LP = g_Yaw;
      g_FirstRun = false;
    }
    
    g_Yaw_LP += (g_Yaw - g_Yaw_LP) * 0.01;
    
    //g_Gx = (int16_t) floor((double) g_Gx_Raw * (double) g_dataScale * 90 / 8192);
    //g_Gy = (int16_t) floor((double) g_Gy_Raw * (double) g_dataScale * 90 / 8192);
    
    g_Gx = (int16_t) floor((double) g_Gx_Raw * (double) g_dataScale * 90 / 8192);
    g_Gy = (int16_t) floor((double) g_Gy_Raw * (double) g_dataScale * 90 / 8192);
    
    
    result.accelData.x = g_Ax / 8192.;
    result.accelData.y = g_Ay / 8192.;
    result.accelData.z = g_Az / 8192.;
    
    result.magData.x = g_Mx;
    result.magData.y = g_My;
    result.magData.z = g_Mz;
    
    result.gyroData.x = g_Gx;
    result.gyroData.y = g_Gy;
    result.gyroData.z = 0.;
    
    result.angle = g_Yaw_LP;
    return result;
  }
  
  ImuData GyroMagSensor::getRawSensorData() {
    ImuData result = {0};
    Sensor_ReadData();
    
    result.accelData.x = g_Ax_Raw;
    result.accelData.y = g_Ay_Raw;
    result.accelData.z = g_Az_Raw;
    
    result.gyroData.x = g_Gx_Raw;
    result.gyroData.y = g_Gy_Raw;
    result.gyroData.z = 0;
    
    result.magData.x = g_Mx_Raw;
    result.magData.y = g_Mx_Raw;
    result.magData.z = g_Mx_Raw;
    
    return result;
  }
  
  bool GyroMagSensor::isAccelStill() {
    return unchangedAccelTimer > ACCEL_TIMER_STILL_VALUE;
  }
  
  void GyroMagSensor::callibrate() {
    
    setMagnetometerCallibration(DEFAULT_MAG_CALLIBRATION_X, DEFAULT_MAG_CALLIBRATION_Y, DEFAULT_MAG_CALLIBRATION_Z);
    //gyroMagSensor.Magnetometer_Calibrate();
    
    
    const int avg_times = 50;
    float xMax = 0, yMax = 0, zMax = 0;
    float xMin = 0, yMin = 0, zMin = 0;
    float xAccel, yAccel, zAccel, gyrx, gyry;
    
    
    unsigned long i;
    for (int j = 0; j < avg_times; j++) {
      i = 65535;
      
      ImuDataf imuData = getSensorData();
      xAccel = imuData.accelData.x;
      yAccel = imuData.accelData.y;
      zAccel = imuData.accelData.z;
      
      gyrx = imuData.gyroData.x;
      gyry = imuData.gyroData.y;
      
      if (gyrx < 0)
        gyrx *= 1;
      if (gyry < 0)
        gyry *= 1;
      
      xAccel -= gyrx / 90.f;
      yAccel -= gyry / 90.f;
      
      if (xAccel > xMax)
        xMax = xAccel;
      if (yAccel > yMax)
        yMax = yAccel;
      if (zAccel > zMax)
        zMax = zAccel;
      
      if (xAccel < xMin)
        xMin = xAccel;
      if (yAccel < yMin)
        yMin = yAccel;
      if (zAccel < zMin)
        zMin = zAccel;
      
      xOffset += xAccel;
      yOffset += yAccel;
      zOffset += yAccel;
      
      while (i--) {
      }
    }
    
    xOffset /= avg_times;
    yOffset /= avg_times;
    zOffset /= avg_times;
    
    xPass = MAX(abs(xOffset - xMin), abs(xOffset - xMax));
    yPass = MAX(abs(yOffset - yMin), abs(yOffset - yMax));
    zPass = MAX(abs(zOffset - zMin), abs(zOffset - zMax));
  }
  
  ImuDataf GyroMagSensor::getProcessedData() {
    ImuDataf result = getSensorData();
    
    
    if (result.gyroData.x < 0)
      result.gyroData.x *= 1;
    
    if (result.gyroData.y < 0)
      result.gyroData.y *= 1;
    
    result.accelData.x -= result.gyroData.x / 90.f;
    result.accelData.y -= result.gyroData.y / 90.f;
    
    result.accelData.x -= xOffset;
    result.accelData.y -= yOffset;
    
    if (abs(result.accelData.x) < xPass)
      result.accelData.x = 0.;
    
    if (abs(result.accelData.y) < yPass)
      result.accelData.y = 0.;
    
    result.accelData.x *= GRAVITY_CONST;
    result.accelData.y *= GRAVITY_CONST;
    
    if (result.accelData.x == prevAccel.x && result.accelData.y == prevAccel.y) {
      unchangedAccelTimer++;
    } else {
      unchangedAccelTimer = 0;
    }
    prevAccel = result.accelData;
    
    return result;
  }
  
}
/* namespace nxpbc */
