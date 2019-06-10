//
// Created by zvone on 21-Feb-19.
//

#ifndef NXPCUPRASPI_NXPCARRASPI_H
#define NXPCUPRASPI_NXPCARRASPI_H


#if defined(__linux__) || defined(WIN32)


#include "NxpCarAbstract.h"
#include <fstream>

namespace nxpbc {


    struct s_setting;
    struct s_control;
    struct s_data;

    class NxpCommunication;

    /**
     * @brief Hlavní třída auta pro Raspberry Pi
     */
    class NxpCar : NxpCarAbstract {

    public:
        NxpCar();

        virtual ~NxpCar();

        virtual void update() override;

    private:

        s_setting *setting_;
        s_control *control_;
        s_data *data_;

        NxpCommunication *communication_;

        std::ofstream *originalImageFile_;
        std::ofstream *normalizedImageFile_;
        std::ofstream *thresholdedImageFile_;
        std::ofstream *csvFile_;


        /**
         * @brief Metoda pro zápis dat do CSV souboru
         * @param csvFile Název souboru
         * @param state Status
         * @param voltage Napětí
         * @param timestampDiff Rozdíl času
         * @param left Levá vzdálenost
         * @param right Pravá vzdálenost
         * @param leftRatio Levý poměr
         * @param rightRatio Pravý poměr
         * @param ratioDiff Rozdíl poměrů
         * @param servoPos Nastavení serva
         */
        virtual void writeToCsvFile(
                std::ofstream &csvFile,
                nxpbc::MotorsState state,
                float voltage,
                int timestampDiff,
                int left,
                int right,
                float leftRatio,
                float rightRatio,
                float ratioDiff,
                int servoPos);


        /**
         * @brief Metoda pro výpis rozdílu časové známky.
         * @param diff Rozdíl aktuální a předchozí časové známky.
         */
        virtual void printTimestampDiff(unsigned long diff);

    };
}

#endif

#endif //NXPCUPRASPI_NXPCARRASPI_H
