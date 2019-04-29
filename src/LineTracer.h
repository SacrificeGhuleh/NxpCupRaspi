//
// Created by Richard Zvonek on 21-Nov-18.
//

#include <utility>
#include <list>

#include "NxpImage.h"
#include "Region.h"

#ifndef NXPCUPRASPI_LINETRACER_H
#define NXPCUPRASPI_LINETRACER_H

namespace nxpbc {

    struct Region;

    class NxpImage;

    /**
     * @brief Třída pro hledání čáry
     */
    class LineTracer {
    public:

        /**
         * @brief Konstruktor třídy
         * @param listSize Počet uchovaných dat v historii
         */
        LineTracer(int listSize);

        /**
         * @brief Přidání obrazu do fronty
         * @param image Obrazová data z kamery
         */
        void addImage(const NxpImage& image, bool forceSearchRegions = false);

        /**
         * @brief Metoda pro získání vzdálenosti levé čáry
         * @return Vzdálenost levé čáry
         */
        unsigned int getLeft();


        /**
         * @brief Metoda pro získání vzdálenosti pravé čáry
         * @return Vzdálenost pravé čáry
         */
        unsigned int getRight();

        Region getLastRawRegion();

        bool hasLeft();
        bool hasRight();

        //private:

        /**
         * @brief Metoda pro výpočet vzdáleností čar
         *
         * Pokud existují vzdálenosti z předchozí iterace, metoda hledá čáry od předchozích čar.
         * Pokud vzdálenosti neexistují a nebo nebyly čáry nalezeny, metoda hledá čáry od okraje pomocí regionů.
         *
         * @param image Obrazová data z kamery
         * @param hasPrevDistance True, pokud existují vzdálenosti z předchozí iterace
         * @return Vzdálenosti čar
         */
        Region getDistances(const NxpImage &image, bool hasPrevDistance, bool forceSearchRegions = false);

        bool findByPreviousIndex(const NxpImage &image, Region &foundRegion);

        std::vector<Region> getRegions(const NxpImage &image, uint8_t leftIndex = 0, uint8_t rightIndex = CAMERA_LINE_LENGTH - 1,  bool saveToClass = true);

        std::vector<nxpbc::Region> currentRegions_;
        /**
         * @brief Velikost historie čar.
         */
        int listSize_;

        bool computedRegion_;
        bool unchangedLeft_;
        bool unchangedRight_;

        bool hasLeft_;
        bool hasRight_;

        uint8_t blackRegionsCount_;
        uint8_t whiteRegionsCount_;

        std::pair<uint8_t, uint8_t> getDistancesPair();

        /**
         * @brief prvni prvek je levy, druhy je pravy
         */
        std::pair<uint8_t, uint8_t> currentAverage_;

        /**
         * @brief prvni prvek je levy, druhy je pravy
         */
        std::pair<uint8_t, uint8_t> currentMedian_;


        void reset();

        size_t getListSize();

        /**
         * @brief Fronta pro uložení čar.
         */
        std::list<Region> imageRegionList_;

    };
}

#endif //NXPCUPRASPI_LINETRACER_H
