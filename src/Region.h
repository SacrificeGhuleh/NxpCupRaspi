//
// Created by Richard Zvonek on 08-Oct-18.
//

#ifndef NXPCUPRASPI_REGION_H
#define NXPCUPRASPI_REGION_H

#include "NxpDefines.h"

namespace nxpbc {
    /**
     * @brief Struktura pro uložení nalezených oblastí v obraze
     */
    struct Region {
        Region(int left = minLeft, int right = maxRight, int color = 0xff) {
            if (left < minLeft) {
                left = minLeft;
            }
            if (right > maxRight) {
                right = maxRight;
            }

            this->left = left;
            this->right = right;
            this->color = color;
        };

        int left;
        int right;
        int color;

        /**
         * @brief Metoda pro získání velikostí oblasti
         * @return Velikost oblasti
         */
        int getSize() const {
            return right - left;
        }

        /**
         * @brief Metoda pro získání středu oblasti
         * @return Střed oblasti
         */
        int getCenter() const {
            return (right + left) / 2;
        }

        const static int minLeft = BLACK_COUNT;
        const static int maxRight = CAMERA_LINE_LENGTH - BLACK_COUNT - 1;
    };
}

#endif //NXPCUPRASPI_REGION_H
