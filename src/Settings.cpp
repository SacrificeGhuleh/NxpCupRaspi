//
// Created by Richard Zvonek on 07-Nov-18.
//

#include "pch.h"
#include "Settings.h"

namespace nxpbc {
    Settings *Settings::getInstance() {
        static Settings *instance;
        if (!instance)
            instance = new Settings();

        return instance;
    }

    Settings::Settings() : NxpCupXmlConfig("config.xml") {
    }

    Settings::~Settings() {

    }
}