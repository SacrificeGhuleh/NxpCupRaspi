//
// Created by Richard Zvonek on 21-Nov-18.
//

#ifndef NXPCUPRASPI_NXPDEFINES_H
#define NXPCUPRASPI_NXPDEFINES_H

namespace nxpbc {

#define SERIAL_DATA_BUFF_SIZE 281
#define DATA_BUFF_SIZE SERIAL_DATA_BUFF_SIZE - 5
#define SERIAL_CONTROL_BUFF_SIZE 17
#define SERIAL_SETTING_BUFF_SIZE 17

/**
 * @brief Černá barva
 */
#define COLOR_BLACK 0x00

/**
 * @brief Zpracovaná bílá barva
 */
#define COLOR_WHITE 0xff

/**
 * @brief Originální bílá barva z kamery
 */
#define COLOR_WHITE_ORIGINAL 0xfff

/**
 * @brief Počet zahozených pixelů od krajů
 */
#define BLACK_COUNT 10
  
  /**
   * @brief Maximální hodnota PWM
   */
#define CONTROL_PWM_MAX 300
  
  /**
   * @brief Hodnota PWM kroku při rozjezu
   */
#define CONTROL_PWM_STEP 20
  
  /**
   * @brief Maximální hodnota PWM pro jízdu
   */
#define CONTROL_DRIVE_MAX 400
  
  /**
   * @brief Nastavení středu serva
   */
#define SERVO_CENTER 1400
  
  /**
   * @brief Maximální natočení serva
   */
#define SERVO_LR 1000
  
  /**
   * @brief Minimální hodnota poměru pro zatáčení
   */
#define CONTROL_TURN_RATIO 0.0f
  
  /**
   * @brief Maximální hodnota poměru pro zatáčení
   */
#define CONTROL_MAX_RATIO 1.0f
  
  /**
   * @brief Proporcionální konstanta pro PID
   */
#define CONST_ERROR 160.f//200.f//160.6f
  /**
   * @brief Derivační konstanta pro PID
   */
#define CONST_DERIVATIVE 8.3f
  /**
   * @brief Integrační konstanta pro PID
   */
#define CONST_INTEGRAL 1.5f
  
  /**
   * @brief Poloměr oblasti, ve které se hledá čára podle předchozích hodnot
   */
#define PREV_LINE_SEARCH_REGION 5
  
  /**
   * @brief Počet snímků pro uložení v paměti
   */
#define TRACER_HISTORY_SIZE 5
  
  /**
   * @brief Konstanta pro převod PWM serva na stupně natočení kol
   */
#define SERVO_TO_DEG_CONST 5.85f

/**
 * @brief Rozvor kol v decimetrech
 * Vzdalenost prednich a zadnich kol
 */
#define ALAMAK_WHEELBASE 1.85f

/**
 * @brief Rozchod kol v decimetrech
 * Vzdalenost leveho a praveho kola
 */
#define ALAMAK_TRACK 1.50f

/**
 * @brief Koeficient pro diferencial
 */
#define TURN_CONTROL_COEFICIENT 100.f

/**
 * @brief Pi
 */
#define PI 3.14159265

/**
 * @brief Čítač pro měření přejetí čáry
 */
#define LINE_CROSS_TIMER 40

/**
 * @brief Prahovací hodnota pro IR senzory
 */
#define WHITE_IR_BOUND 2500

/**
 * @brief Počet kroků při rozjezdu
 */
#define START_STEPS 200

/**
 * @brief Maximální počet uložených regionů
 */
#define MAX_REGIONS_COUNT 25

/**
 * @brief Konstanta pro rozpoznání nízké diverzity obrazu
 */
#define LOW_DIVERSITY 0

/**
 * @brief Makro pro safe delete
 */
#define DELETE_AND_PARK(ptr)  if(\
    ptr != nullptr){\
    delete ptr;\
    ptr = nullptr;\
    }
}

//Arduino.h
#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif

#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif
#ifndef bit
#define bit(b) (1UL << (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#endif //NXPCUPRASPI_NXPDEFINES_H
