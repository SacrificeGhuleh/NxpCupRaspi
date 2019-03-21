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
        LineTracer(const int listSize);

        /**
         * @brief Přidání obrazu do fronty
         * @param image Obrazová data z kamery
         */
        void addImage(NxpImage image);


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
        Region getDistances(const NxpImage &image, bool hasPrevDistance);

        bool findByPreviousIndex(const NxpImage &image, Region &foundRegion);


        std::vector<nxpbc::Region> currentRegions_;
        /**
         * @brief Velikost historie čar.
         */
        int listSize_;

        bool computedRegion_;
        bool unchangedLeft_;
        bool unchangedRight_;

        void reset();

        /**
         * @brief Fronta pro uložení čar.
         */

#if defined(__linux__) || defined(WIN32)
        std::list<std::pair<NxpImage, Region>> imageRegionList_;
#endif
#if defined(__MCUXPRESSO)
        std::list<Region> imageRegionList_;
#endif

    };
}

#endif //NXPCUPRASPI_LINETRACER_H
