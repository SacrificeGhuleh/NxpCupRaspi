//
// Created by Richard Zvonek on 21-Nov-18.
//

#include "pch.h"

#include "LineTracer.h"
#include "Settings.h"
#include "Logger.h"
#include "ImageType.h"

namespace nxpbc {

    LineTracer::LineTracer(const int listSize) : listSize_{listSize} {
    }

    void LineTracer::addImage(const NxpImage image) {
        bool hasPrevLine = true;
        if (imageRegionList_.size() > listSize_) {
            imageRegionList_.pop_front();
        }

        if (imageRegionList_.empty())
            hasPrevLine = false;

        //Region biggestWhiteRegion = getDistances(image, hasPrevLine);

#if defined(__linux__) || defined(WIN32)
        imageRegionList_.emplace_back(image, getDistances(image, hasPrevLine));
#endif

#if defined(__MCUXPRESSO)
        imageRegionList_.emplace_back(getDistances(image, hasPrevLine));
#endif
    }

    Region LineTracer::getDistances(const NxpImage &image, bool hasPrevDistance) {
        std::vector<nxpbc::Region> regions;
        nxpbc::Region biggestWhiteRegion;

        int right, left;

        bool foundLeft = false;
        bool foundRight = false;
        uint8_t newLeftIndex = 0;
        uint8_t newRightIndex = 0;


        if (hasPrevDistance) {
            /*
             * OPRAVIT
             * BUG
             * */

#if defined(__linux__) || defined(WIN32)
            Region r = imageRegionList_.back().second;
#endif
#if defined(__MCUXPRESSO)
            Region r = imageRegionList_.back();
#endif
            int prevLeftIndex = r.left;
            int prevRightIndex = r.right;
            int counter = 0;
            int i = 0;
            while (std::abs(i) < PREV_LINE_SEARCH_REGION) {
                counter++;
                /*
                 * Ochrana proti zacykleni
                 * */
                if (counter - 1 > PREV_LINE_SEARCH_REGION * 2) {
                    break;
                }

                /*NXP_TRACEP("Hledam podle predchozi vzdalenosti, i: %d"
                                   NL, i);*/
                if (foundLeft && foundRight) {
                    break;
                }

                if (!foundLeft) {
                    newLeftIndex = static_cast<uint8_t>(prevLeftIndex + i);
                }

                if (!foundRight) {
                    newRightIndex = static_cast<uint8_t>(prevRightIndex + i);
                }

                if (newLeftIndex <= 0 ||
                    newLeftIndex >= CAMERA_LINE_LENGTH - 1 ||
                    newRightIndex <= 0 ||
                    newRightIndex >= CAMERA_LINE_LENGTH - 1) {
                    continue;
                }

                if (!foundLeft) {
                    uint16_t leftLeftColor = image.at(
                            newLeftIndex,
                            ImgType::Thresholded);
                    uint16_t rightLeftColor = image.at(
                            static_cast<const uint8_t>(newLeftIndex + 1),
                            ImgType::Thresholded);

                    if (leftLeftColor == COLOR_BLACK && rightLeftColor == COLOR_WHITE) {
                        NXP_TRACEP("Nalezen levy index: %d"
                                           NL, newLeftIndex);
                        foundLeft = true;
                    }
                }

                if (!foundRight) {
                    uint16_t leftRightColor = image.at(
                            static_cast<const uint8_t>(newRightIndex - 1),
                            ImgType::Thresholded);

                    uint16_t rightRightColor = image.at(
                            newRightIndex,
                            ImgType::Thresholded);

                    if (rightRightColor == COLOR_BLACK && leftRightColor == COLOR_WHITE) {
                        NXP_TRACEP("Nalezen pravy index: %d"
                                           NL, newRightIndex);
                        foundRight = true;
                    }
                }

                i *= -1;
                if (i >= 0) {
                    i++;
                }
            }
        }

        if (!(foundLeft && foundRight)) {
            NXP_TRACE("Hledam indexy podle regionu"
                              NL);
            uint8_t currentColor = static_cast<uint8_t>(image.at(Region::minLeft, ImgType::Thresholded));
            regions.emplace_back(nxpbc::Region({Region::minLeft, Region::minLeft, currentColor}));
            for (uint8_t i = Region::minLeft; i <= Region::maxRight; i++) {
                /*NXP_TRACEP("idx: %03d\tpix: %03d"
                                   NL, i, image.at(i, ImgType::Thresholded));*/
                if (currentColor != image.at(i, ImgType::Thresholded)) {
                    regions.at(regions.size() - 1).right = i;
                    regions.emplace_back(nxpbc::Region({i, i, image.at(i, ImgType::Thresholded)}));
                }
                currentColor = static_cast<uint8_t>(image.at(i, ImgType::Thresholded));
            }
            regions.at(regions.size() - 1).right = Region::maxRight;
            regions.at(0).left = Region::minLeft;

            //biggestWhiteRegion = regions.at(0);
            biggestWhiteRegion = {Region::minLeft, Region::minLeft, COLOR_WHITE};

            uint8_t whiteRegionsCount = 0;
            uint8_t blackRegionsCount = 0;

            for (nxpbc::Region &r : regions) {
                if (r.color == COLOR_WHITE) {
                    whiteRegionsCount++;
                    if ((r.getSize() > biggestWhiteRegion.getSize())) {
                        biggestWhiteRegion = r;
                    } else {
                        blackRegionsCount++;
                    }
                }
            }

            NXP_INFOP("Pocet nalezenych oblasti: \tBILE: %d\tCERNE: %d"
                              NL, whiteRegionsCount, blackRegionsCount);

            NXP_INFOP("Nalezena nejvetsi bila oblast: Levy index: %d, Pravy index: %d, Stred %d, Velikost %d"
                              NL,
                      biggestWhiteRegion.left,
                      biggestWhiteRegion.right,
                      biggestWhiteRegion.getCenter(),
                      biggestWhiteRegion.getSize());

            newLeftIndex = static_cast<uint8_t>(biggestWhiteRegion.left);
            newRightIndex = static_cast<uint8_t>(biggestWhiteRegion.right);
        } else {
            NXP_TRACE("Nalezeny indexy pomoci starych hodnot"
                              NL);
        }


        left = newLeftIndex;
        right = newRightIndex;

        /*
         * Akce workaround
         * */

        if (left < 10) {
            left = 10;
        }

        if (right < 10) {
            right = 10;
        }

        if (left > CAMERA_LINE_LENGTH - 1 - 10) {
            left = CAMERA_LINE_LENGTH - 1 - 10;
        }

        if (right > CAMERA_LINE_LENGTH - 1 - 10) {
            right = CAMERA_LINE_LENGTH - 1 - 10;
        }


        for (Region &region : regions) {
            NXP_TRACEP("Region: %s velikost: %03d, stred: %03d"
                               NL, region.color == COLOR_BLACK ? "B" : "W", region.getSize(), region.getCenter());
        }
        return {left, right};
    }

    unsigned int LineTracer::getRight() const {
#if defined(__linux__) || defined(WIN32)
        return static_cast<unsigned int>(imageRegionList_.back().second.right);
#endif
#if defined(__MCUXPRESSO)
        return static_cast<unsigned int>(imageRegionList_.back().right);
#endif
    }

    unsigned int LineTracer::getLeft() const {
#if defined(__linux__) || defined(WIN32)
        return static_cast<unsigned int>(imageRegionList_.back().second.left);
#endif
#if defined(__MCUXPRESSO)
        return static_cast<unsigned int>(imageRegionList_.back().left);
#endif
    }

}
