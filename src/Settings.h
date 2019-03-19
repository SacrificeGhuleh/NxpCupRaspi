//
// Created by Richard Zvonek on 07-Nov-18.
//

#ifndef NXPCUPRASPI_SETTINGS_H
#define NXPCUPRASPI_SETTINGS_H
#if defined(__linux__) || defined(WIN32)
#include "NxpCupXmlConfig.h"

namespace nxpbc {
    class Settings : public NxpCupXmlConfig {
    public:
        /**
         * @brief Singleton třída pro nastavení
         * @return Instance třídy nastavení
         */
        static Settings *getInstance();

    private:
        Settings();

        virtual ~Settings();
    };
}
#endif
#endif //NXPCUPRASPI_SETTINGS_H
