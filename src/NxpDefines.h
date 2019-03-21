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

#ifndef __TFC_H

    /*
     * Visual studio
     */

#define CAMERA_LINE_LENGTH			128				// Number of camera pixels

#define PWM_MINMAX					1000			// -/+ range for PWM

#define SERVO_MINMAX				1000			// -/+ range for servo position

#define ADC_MAXVAL					0x0FFF			// Maximum value of analog sample from ADC
#define ANDATA_MINMAX				1000			// -/+ range for analog values (e.g. potentiometers)

#define SERVO_DEFAULT_CENTER		1500			// Center position of servo - pulse in microseconds
#define SERVO_DEFAULT_MAX_LR		200				// Default -/+ range of pulse width range
#define SERVO_MAX_LR				400				// Maximum -/+ allowed range of pulse width

#define PWM_DEFAULT_MAX				200				// Default value for maximal -/+ PWM duty cycle
#define PWM_MAX						500				// Maximal allowed -/+ PWM duty cycle

    // commands
#define CMD_DATA					1
#define CMD_SETTING					2
#define CMD_CONTROL					3

    // protocol start and stop byte
#define STX							0x2
#define ETX							0x3

#ifndef andata_chnl_enum 
enum andata_chnl_enum { anPOT_1, anPOT_2, anFB_A, anFB_B, anBAT, anLast };
#endif

#else

#ifndef CAMERA_LINE_LENGTH
#define CAMERA_LINE_LENGTH			TFC_CAMERA_LINE_LENGTH				// Number of camera pixels
#endif

#ifndef PWM_MINMAX
#define PWM_MINMAX					TFC_PWM_MINMAX			// -/+ range for PWM
#endif

#ifndef SERVO_MINMAX
#define SERVO_MINMAX				TFC_SERVO_MINMAX			// -/+ range for servo position
#endif

#ifndef ADC_MAXVAL
#define ADC_MAXVAL					TFC_ADC_MAXVAL			// Maximum value of analog sample from ADC
#endif

#ifndef ANDATA_MINMAX
#define ANDATA_MINMAX				TFC_ANDATA_MINMAX			// -/+ range for analog values (e.g. potentiometers)
#endif

#ifndef SERVO_DEFAULT_CENTER
#define SERVO_DEFAULT_CENTER		TFC_SERVO_DEFAULT_CENTER			// Center position of servo - pulse in microseconds
#endif
#ifndef SERVO_DEFAULT_MAX_LR
#define SERVO_DEFAULT_MAX_LR		TFC_SERVO_DEFAULT_MAX_LR				// Default -/+ range of pulse width range
#endif

#ifndef SERVO_MAX_LR
#define SERVO_MAX_LR				TFC_SERVO_MAX_LR				// Maximum -/+ allowed range of pulse width
#endif

#ifndef PWM_DEFAULT_MAX
#define PWM_DEFAULT_MAX				TFC_PWM_DEFAULT_MAX				// Default value for maximal -/+ PWM duty cycle
#endif

#ifndef PWM_MAX
#define PWM_MAX						HW_TFC_PWM_MAX				// Maximal allowed -/+ PWM duty cycle
#endif

#endif


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


#if defined(__linux__) || defined(WIN32)
/**
 * @brief Název sériového portu
 */
#define COM_PORT_NAME Settings::getInstance()->getComPortName().c_str()

/**
 * @brief Baud rate sériového portu
 */
#define COM_BAUD_RATE Settings::getInstance()->getComPortBaudRate()

/**
 * @brief Timeout sériového portu
 */
#define COM_TIMEOUT Settings::getInstance()->getComPortTimeout()

/**
 * @brief Maximální hodnota PWM
 */
#define CONTROL_PWM_MAX Settings::getInstance()->getControlPwmMax()

/**
 * @brief Hodnota PWM kroku při rozjezu
 */
#define CONTROL_PWM_STEP Settings::getInstance()->getControlPwmStep()

/**
 * @brief Maximální hodnota PWM pro jízdu
 */
#define CONTROL_DRIVE_MAX Settings::getInstance()->getControlDriveMax()

/**
 * @brief Nastavení středu serva
 */
#define SERVO_CENTER Settings::getInstance()->getServoCenter()

/**
 * @brief Maximální natočení serva
 */
#define SERVO_LR Settings::getInstance()->getServoLR()

/**
 * @brief Minimální hodnota poměru pro zatáčení
 */
#define CONTROL_TURN_RATIO Settings::getInstance()->getServoTurnRatio()

/**
 * @brief Maximální hodnota poměru pro zatáčení
 */
#define CONTROL_MAX_RATIO Settings::getInstance()->getServoMaxRatio()

/**
 * @brief TODO
 */
#define CONST_ERROR Settings::getInstance()->getErrorConst()
/**
 * @brief TODO
 */
#define CONST_DERIVATIVE Settings::getInstance()->getDerivativeConst()
/**
 * @brief TODO
 */
#define CONST_INTEGRAL Settings::getInstance()->getIntegralConst()

/**
 * @brief Poloměr oblasti, ve které se hledá čára podle předchozích hodnot
 */
#define PREV_LINE_SEARCH_REGION Settings::getInstance()->getPrevLineSearchRegion()

/**
 * @brief Ukládání originálního záznamu
 */
#define SAVE_RAW Settings::getInstance()->getSaveRaw()

/**
 * @brief Ukládání normalizovaného záznamu
 */
#define SAVE_NORM Settings::getInstance()->getSaveNorm()

/**
 * @brief Ukládání oříznutého záznamu
 */
#define SAVE_THRESH Settings::getInstance()->getSaveThresh()

/**
 * @brief Ukládání hodnot do CSV souboru
 */
#define SAVE_CSV Settings::getInstance()->getSaveCsv()
#endif

#if defined(__MCUXPRESSO)


    /**
     * @brief Maximální hodnota PWM
     */
#define CONTROL_PWM_MAX 1000

    /**
     * @brief Hodnota PWM kroku při rozjezu
     */
#define CONTROL_PWM_STEP 100

    /**
     * @brief Maximální hodnota PWM pro jízdu
     */
#define CONTROL_DRIVE_MAX 500

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
     * @brief TODO
     */
#define CONST_ERROR 10.
    /**
     * @brief TODO
     */
#define CONST_DERIVATIVE 0.
    /**
     * @brief TODO
     */
#define CONST_INTEGRAL 0.

    /**
     * @brief Poloměr oblasti, ve které se hledá čára podle předchozích hodnot
     */
#define PREV_LINE_SEARCH_REGION 10



#endif

#define MAX_REGIONS_COUNT 25

/**
 * @brief Makro pro safe delete
 */
#define DELETE_AND_PARK(ptr)  if(\
    ptr != nullptr){\
    delete ptr;\
    ptr = nullptr;\
    }
}

#endif //NXPCUPRASPI_NXPDEFINES_H
