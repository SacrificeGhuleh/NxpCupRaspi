//
// Created by zvone on 06-Apr-19.
//

#ifndef NXPCUPRASPI_VEC3_H
#define NXPCUPRASPI_VEC3_H

#include <cstdint>

namespace nxpbc {
    /**
     * @brief Třísložkový celočíselný vektor
     */
    union Vec3 {
        struct {
            int16_t x, y, z;
        };
        int16_t val[3];

        Vec3 operator-(const Vec3 &rhs) {
            return {static_cast<int16_t>(this->x - rhs.x),
                    static_cast<int16_t>(this->y - rhs.y),
                    static_cast<int16_t>(this->z - rhs.z)};
        }

        Vec3 operator+(const Vec3 &rhs) {
            return {static_cast<int16_t>(this->x + rhs.x),
                    static_cast<int16_t>(this->y + rhs.y),
                    static_cast<int16_t>(this->z + rhs.z)};
        }

        bool operator==(const Vec3 &rhs) {
            return this->x == rhs.x &&
                   this->y == rhs.y &&
                   this->z == rhs.z;
        }

        bool operator!=(const Vec3 &rhs) {
            return !(*this == rhs);
        };

    };

    /**
     * @brief Třísložkový neceločíselný vektor
     */
    union Vec3f {
        struct {
            double x, y, z;
        };
        double val[3];

        Vec3f operator-(const Vec3f &rhs) {
            return {static_cast<double>(this->x - rhs.x),
                    static_cast<double>(this->y - rhs.y),
                    static_cast<double>(this->z - rhs.z)};
        }

        Vec3f operator+(const Vec3f &rhs) {
            return {static_cast<double>(this->x + rhs.x),
                    static_cast<double>(this->y + rhs.y),
                    static_cast<double>(this->z + rhs.z)};
        }

        bool operator==(const Vec3f &rhs) {
            return this->x == rhs.x &&
                   this->y == rhs.y &&
                   this->z == rhs.z;
        }

        bool operator!=(const Vec3f &rhs) {
            return !(*this == rhs);
        };

    };
}

#endif //NXPCUPRASPI_VEC3_H
