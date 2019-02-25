//
// Created by Richard Zvonek on 07-Nov-18.
//

#include "pch.h"
#include "NxpCupXmlConfig.h"

namespace nxpbc {
    NxpCupXmlConfig::NxpCupXmlConfig(const std::string &filename) :
            XmlReader(filename),
            configXml_{getChildString(xmlFile_, "config")},
            comXml_{getChildString(configXml_, "comPort")},
            nxpConfigXml_{getChildString(configXml_, "nxpConfig")},
            comPortName_{getChildString(comXml_, "Name")},
            comPortBaudRate_{getChildInt(comXml_, "Baud")},
            comPortTimeout_{getChildInt(comXml_, "Timeout")},
            controlPwmMax_{getChildInt(nxpConfigXml_, "ControlPwmMax")},
            controlPwmStep_{getChildInt(nxpConfigXml_, "ControlPwmStep")},
            controlDriveMax_{getChildInt(nxpConfigXml_, "ControlDriveMax")},
            servoCenter_{getChildInt(nxpConfigXml_, "ServoCenter")},
            servoLR_{getChildInt(nxpConfigXml_, "ServoLR")},
            errorConst_{getChildFloat(nxpConfigXml_, "ErrorConst")},
            derivativeConst_{getChildFloat(nxpConfigXml_, "DerivativeConst")},
            integralConst_{getChildFloat(nxpConfigXml_, "IntegralConst")},
            servoTurnRatio_{getChildFloat(nxpConfigXml_, "ServoTurnRatio")},
            servoMaxRatio_{getChildFloat(nxpConfigXml_, "ServoMaxRatio")},
            prevLineSearchRegion_{getChildInt(nxpConfigXml_, "PrevLineSearchRegion")},
            saveRaw_{static_cast<bool>(getChildInt(nxpConfigXml_, "SaveRaw"))},
            saveNorm_{static_cast<bool>(getChildInt(nxpConfigXml_, "SaveNorm"))},
            saveThresh_{static_cast<bool>(getChildInt(nxpConfigXml_, "SaveThresh"))},
            saveCsv_{static_cast<bool>(getChildInt(nxpConfigXml_, "SaveCsv"))} {}

    const std::string &NxpCupXmlConfig::getConfigXml() const {
        return configXml_;
    }

    const std::string &NxpCupXmlConfig::getComXml() const {
        return comXml_;
    }

    const std::string &NxpCupXmlConfig::getNxpConfigXml() const {
        return nxpConfigXml_;
    }

    const std::string &NxpCupXmlConfig::getComPortName() const {
        return comPortName_;
    }

    const int NxpCupXmlConfig::getComPortBaudRate() const {
        return comPortBaudRate_;
    }

    const int NxpCupXmlConfig::getControlPwmMax() const {
        return controlPwmMax_;
    }

    const int NxpCupXmlConfig::getControlPwmStep() const {
        return controlPwmStep_;
    }

    const int NxpCupXmlConfig::getServoCenter() const {
        return servoCenter_;
    }

    const int NxpCupXmlConfig::getServoLR() const {
        return servoLR_;
    }

    const float NxpCupXmlConfig::getServoTurnRatio() const {
        return servoTurnRatio_;
    }

    const float NxpCupXmlConfig::getServoMaxRatio() const {
        return servoMaxRatio_;
    }

    const int NxpCupXmlConfig::getPrevLineSearchRegion() const {
        return prevLineSearchRegion_;
    }

    const bool NxpCupXmlConfig::getSaveRaw() const {
        return saveRaw_;
    }

    const bool NxpCupXmlConfig::getSaveNorm() const {
        return saveNorm_;
    }

    const bool NxpCupXmlConfig::getSaveThresh() const {
        return saveThresh_;
    }

    const bool NxpCupXmlConfig::getSaveCsv() const {
        return saveCsv_;
    }

    const int NxpCupXmlConfig::getControlDriveMax() const {
        return controlDriveMax_;
    }

    const int NxpCupXmlConfig::getComPortTimeout() const {
        return comPortTimeout_;
    }

    const float NxpCupXmlConfig::getErrorConst() const {
        return errorConst_;
    }

    const float NxpCupXmlConfig::getDerivativeConst() const {
        return derivativeConst_;
    }

    const float NxpCupXmlConfig::getIntegralConst() const {
        return integralConst_;
    }

}