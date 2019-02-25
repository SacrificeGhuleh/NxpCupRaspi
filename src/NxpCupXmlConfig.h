//
// Created by Richard Zvonek on 07-Nov-18.
//

#ifndef NXPCUPRASPI_NXPCUPXMLCONFIG_H
#define NXPCUPRASPI_NXPCUPXMLCONFIG_H

#include "XmlReader.h"

namespace nxpbc {
    /**
     * @brief Třída pro mapování XML dokumentu NXP CUP
     */
    class NxpCupXmlConfig : XmlReader {
    public:
        /**
         * @brief Konstruktor třídy NxpCupXmlConfig
         * @param filename Název souboru
         */
        explicit NxpCupXmlConfig(const std::string &filename);

    private:
        const std::string configXml_;
        const std::string comXml_;
        const std::string nxpConfigXml_;

        const std::string comPortName_;
        const int comPortBaudRate_;
        const int comPortTimeout_;

    private:

        const int controlPwmMax_;
        const int controlPwmStep_;
        const int controlDriveMax_;
        const int servoCenter_;
        const int servoLR_;
        const float errorConst_;
        const float derivativeConst_;
        const float integralConst_;
        const float servoTurnRatio_;
        const float servoMaxRatio_;

        const int prevLineSearchRegion_;

        const bool saveRaw_;
        const bool saveNorm_;
        const bool saveThresh_;
        const bool saveCsv_;

        /**
         * @brief
         * @return
         */
        const std::string &getConfigXml() const;

        /**
         * @brief
         * @return
         */
        const std::string &getComXml() const;

        /**
         * @brief
         * @return
         */
        const std::string &getNxpConfigXml() const;

    public:
        const float getErrorConst() const;

        const float getDerivativeConst() const;

        const float getIntegralConst() const;

        /**
         * @brief
         * @return
         */
        const std::string &getComPortName() const;

        /**
         * @brief
         * @return
         */
        const int getComPortBaudRate() const;

        /**
         * @brief
         * @return
         */
        const int getComPortTimeout() const;

        /**
         * @brief
         * @return
         */
        const int getControlPwmMax() const;

        /**
         * @brief
         * @return
         */
        const int getControlPwmStep() const;

        /**
         * @brief
         * @return
         */
        const int getControlDriveMax() const;

        /**
         * @brief
         * @return
         */
        const int getServoCenter() const;

        /**
         * @brief
         * @return
         */
        const int getServoLR() const;

        /**
         * @brief
         * @return
         */
        const float getServoTurnRatio() const;

        /**
         * @brief
         * @return
         */
        const float getServoMaxRatio() const;

        /**
         * @brief
         * @return
         */
        const int getPrevLineSearchRegion() const;

        /**
         * @brief
         * @return
         */
        const bool getSaveRaw() const;

        /**
         * @brief
         * @return
         */
        const bool getSaveNorm() const;

        /**
         * @brief
         * @return
         */
        const bool getSaveThresh() const;

        /**
         * @brief
         * @return
         */
        const bool getSaveCsv() const;

    };
}
#endif //NXPCUPRASPI_NXPCUPXMLCONFIG_H
