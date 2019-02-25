//
// Created by Richard Zvonek on 19-Sep-18.
//

#ifndef NXPCUPRASPI_MOTORSSTATE_H
#define NXPCUPRASPI_MOTORSSTATE_H


namespace nxpbc {
    /**
     * @brief Enum pro rozliseni jednotlivych modu jizdy
     */
    enum class MotorsState {
        /**
         * @brief Mod pro stani
         */
                Stay,
        /**
         * @brief Mod pro rozjezd
         */
                Start,
        /**
         * @brief Mod pro jizdu
         */
                Ride
    };
}

#endif //NXPCUPRASPI_MOTORSSTATE_H
