/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    main.cpp
 * @brief   Application entry point.
 */

#include "pch.h"
#include <cstdlib>
#include <new>
#include <ctime>
#include "Logger.h"
#include "enet.h"
#include "NxpCarFreescale.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include "enet.h"
#include "tfc_k6xf.h"
#include "tfc.h"
#include "NxpSendFrame.h"
#include "GyroMagSensor.h"

#include "motorsState.h"

using namespace nxpbc;

nxpbc::NxpCar *car = nullptr;
TFC *tfc = nullptr;
Enet enet;
nxpbc::GyroMagSensor gyroMagSensor;

nxpbc::ImuDataf imuData;
float xAccel, yAccel, zAccel, gyrx, gyry, max = 1, min = -1;

float xSpeed = 0;
float ySpeed = 0;

float xOffset = 0, yOffset = 0;

float xPass = 0, yPass = 0;

uint32_t prevTime = 0, curTime = 0;
float deltaTime = 0;


#define DIAG_PRINTF PRINTF

#define LED_HYST 5
#define LED_CENTER(pot) ( ( pot ) < -(LED_HYST) ? 0x01 : ( ( pot ) > (LED_HYST) ? 0x2 : 0x03 ) )

#define UDP_SEND_DATA 0

void sendData();

void diagnostic();

void callibrateImu();


int main() {
  /* Init board hardware. */
  
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();
  /* Init FSL debug console. */
  BOARD_InitDebugConsole();
  
  gyroMagSensor.BOARD_I2C_ReleaseBus();
  
  BOARD_I2C_ConfigurePins();
  uint32_t old_tick = 0;
  
  BOARD_Accel_I2C_Init();
  
  /* Disable SYSMPU. */
  SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;
  /* Set RMII clock src. */
  SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;
  
  /*
   * IMU IIC INIT
   * */
  gyroMagSensor.init();
  
  
  /*
   * Car INIT
   * */
  car = new nxpbc::NxpCar();
  tfc = car->getTfc();
  /*
   * TFC INIT
   * */
  tfc->InitAll();
  tfc->MotorPWMOnOff(0b11);
  tfc->ServoOnOff(0b11);
  tfc->setServoCalibration(0, TFC_SERVO_DEFAULT_CENTER, 500);
  tfc->setPWMMax(CONTROL_PWM_MAX);
  
  
  /*
   * UDP INIT
   * */
#if UDP_SEND_DATA
  tfc->setLED(0, 1);
  tfc->setLED(1, 1);
  tfc->setLED(2, 1);
  tfc->setLED(3, 1);
  
  PRINTF("Initializing enet...");
  
  enet.init(sizeof(nxpbc::SendData), 4444);
#endif
  
  
  /*
   * IMU CALLIBRATION
   * */
  callibrateImu();
  
  for (;;) {
    enet.check();
    // If DIP switch 1 is low, then drive car, else Demo program
    if (tfc->getDIPSwitch() & 0x01) {
      // Run Demo Program
      if (HW_TFC_TimeStamp != old_tick) {
        diagnostic();
        old_tick = HW_TFC_TimeStamp;
      }
    } else {
      if (HW_TFC_TimeStamp != old_tick) {
        car->update();
        if (gyroMagSensor.isAccelStill() && car->getMotorsState() == MotorsState::Brake) {
          car->switchToStayMode();
        }
        old_tick = HW_TFC_TimeStamp;
        if ((old_tick % 5) == 0) {
          if (car->getMotorsState() == nxpbc::MotorsState::Stay) {
            xSpeed = 0.f;
            ySpeed = 0.f;
          }
          sendData();
        }
        
        prevTime = curTime;
      }
    }
  } // end of infinite for loop
}

void callibrateImu() {
  tfc->setLED(0, 1);
  tfc->setLED(1, 0);
  tfc->setLED(2, 1);
  tfc->setLED(3, 0);
  
  gyroMagSensor.callibrate();
  
  tfc->setLED(0, 0);
  tfc->setLED(1, 0);
  tfc->setLED(2, 0);
  tfc->setLED(3, 0);
  
}

void sendData() {
  
  curTime = getMs();
  deltaTime = (curTime - prevTime);// / 1000.f; //ms to s
  
  /*IMU DATA*/
  
  imuData = gyroMagSensor.getProcessedData();
  
  xAccel = imuData.accelData.x;
  yAccel = imuData.accelData.y;
  zAccel = imuData.accelData.z;
  
  gyrx = imuData.gyroData.x;
  gyry = imuData.gyroData.y;
  
  xSpeed += xAccel * deltaTime;
  ySpeed += yAccel * deltaTime;
  
  /*IMU DATA*/
  for (int i = 0; i < 3; i++) {
    car->sendData_.accelValues[i] = imuData.accelData.val[i];
    car->sendData_.gyroValues[i] = imuData.gyroData.val[i];
    car->sendData_.magnetoValues[i] = imuData.magData.val[i];
  }
  car->sendData_.angle = imuData.angle;
  
  car->sendData_.accelValues[0] = xAccel;
  car->sendData_.accelValues[1] = yAccel;
  
  car->sendData_.magnetoValues[0] = xSpeed;
  car->sendData_.magnetoValues[1] = ySpeed;
  
  enet.send(&car->sendData_, sizeof(nxpbc::SendData));
}

void diagnostic() {
  tfc->ServoOnOff(1);
  
  switch (tfc->getDIPSwitch() >> 1) {
    case 0: // Switch A-B & Potentiometers & test LEDs
    {
      tfc->setServo_i(0, 0);
      tfc->MotorPWMOnOff(0);
      
      DIAG_PRINTF("SwA: %d SwB: %d Pot1: %5d Pot2: %5d BAT: %5.2f" NL,
                  tfc->getPushButton(0), tfc->getPushButton(1), tfc->ReadPot_i(0),
                  tfc->ReadPot_i(1), tfc->ReadBatteryVoltage_f());
      uint8_t led = 0;
      if (tfc->getPushButton(0))
        led |= 0x3;
      if (tfc->getPushButton(1))
        led |= 0xC;
      tfc->setLEDs(led);
      break;
    }
    case 1: // Test Servo
    {
      tfc->MotorPWMOnOff(0);
      
      int pa = tfc->ReadPot_i(0);
      int pb = tfc->ReadPot_i(1);
      
      tfc->setLEDs(LED_CENTER(pa) | (LED_CENTER(pb) << 2));
      DIAG_PRINTF("DutyCycle: %4d %4d" NL, TFC_SERVO_DEFAULT_CENTER + pa,
                  TFC_SERVO_DEFAULT_CENTER + pb);
      
      // Workaround for diagnostic purpose only! Do not use it for control!
      // Temporarily enable full servo control
      tfc_setting_s curset = tfc->m_setting;
      tfc->m_setting.servo_center[0] = TFC_SERVO_DEFAULT_CENTER + pa;
      tfc->m_setting.servo_max_lr[0] = TFC_ANDATA_MINMAX;
      tfc->m_setting.servo_center[1] = TFC_SERVO_DEFAULT_CENTER + pb;
      tfc->m_setting.servo_max_lr[1] = TFC_ANDATA_MINMAX;
      
      tfc->setServo_i(0, 0);
      tfc->setServo_i(1, 0);
      
      // restore safe control
      tfc->m_setting = curset;
      
      break;
    }
    case 2: // Test Motors
    {
      tfc->setServo_i(0, 0);
      tfc->MotorPWMOnOff(1);
      
      int pa = tfc->ReadPot_i(0);
      int pb = tfc->ReadPot_i(1);
      tfc->setLEDs(LED_CENTER(pa) | (LED_CENTER(pb) << 2));
      DIAG_PRINTF("Pot1: %5d Pot2: %5d FB-A: %6.2f FB-B: %6.2f" NL, pa, pb,
                  tfc->ReadFB_f(0), tfc->ReadFB_f(1));
      
      tfc_setting_s curset = tfc->m_setting;
      tfc->m_setting.pwm_max = HW_TFC_PWM_MAX;
      
      tfc->setMotorPWM_i(pa, pb);
      
      tfc->m_setting = curset;
      break;
    }
    case 4: // Test Camera
    {
      tfc->setServo_i(0, 0);
      tfc->MotorPWMOnOff(0);
      tfc->setLEDs(0);
      
      uint16_t line[TFC_CAMERA_LINE_LENGTH];
      
      tfc->getImage(0, line, TFC_CAMERA_LINE_LENGTH);
      
      DIAG_PRINTF("Line:" NL);
      for (uint16_t i = 0; i < TFC_CAMERA_LINE_LENGTH; i++) {
        if (i % 16 == 0)
          DIAG_PRINTF(NL);
        DIAG_PRINTF(" %03X", line[i]);
      }
      DIAG_PRINTF(NL);
      break;
    }
    default: tfc->setServo_i(0, 0);
      tfc->MotorPWMOnOff(0);
  }
}
