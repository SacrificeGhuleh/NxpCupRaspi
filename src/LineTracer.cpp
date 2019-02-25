//
// Created by Richard Zvonek on 21-Nov-18.
//

#include "pch.h"

#include "LineTracer.h"
#include "NxpImage.h"
#include "Region.h"
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

        Region biggestWhiteRegion = getDistances(image, hasPrevLine);

        imageRegionList_.emplace_back(image, biggestWhiteRegion);
    }

    Region LineTracer::getDistances(const NxpImage &image, bool hasPrevDistance) {
        std::vector<nxpbc::Region> regions;
        //nxpbc::Region biggestWhiteRegion = nxpbc::Region();
        nxpbc::Region biggestWhiteRegion;

        int right, left;
        //const uint8_t black = 0x00;
        //const uint8_t white = 0xff;
        //const int imgCenter = CAMERA_LINE_LENGTH / 2;

        bool foundLeft = false;
        bool foundRight = false;

//        int newLeftIndex;
//        int newRightIndex;

        uint8_t newLeftIndex = 0;
        uint8_t newRightIndex = 0;


        if (hasPrevDistance) {
            /*
             * OPRAVIT
             * BUG
             * */
            Region r = imageRegionList_.back().second;
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

                NXP_TRACEP("Hledam podle predchozi vzdalenosti, i: %d"
                                   NL, i);
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
                    uint16_t leftLeftColor = image.at(newLeftIndex, ImgType::Thresholded);
                    uint16_t rightLeftColor = image.at(static_cast<const uint8_t>(newLeftIndex + 1),
                                                       ImgType::Thresholded);

                    if (leftLeftColor == COLOR_BLACK && rightLeftColor == COLOR_WHITE) {
                        NXP_TRACEP("Nalezen levy index: %d"
                                           NL, newLeftIndex);
                        foundLeft = true;
                    }
                }

                if (!foundRight) {
                    uint16_t leftRightColor = image.at(static_cast<const uint8_t>(newRightIndex - 1),
                                                       ImgType::Thresholded);
                    uint16_t rightRightColor = image.at(newRightIndex, ImgType::Thresholded);

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
            auto currentColor = static_cast<uint8_t>(image.at(0, ImgType::Thresholded));
            regions.emplace_back(nxpbc::Region({0, 0, currentColor}));
            for (uint8_t i = 0; i < CAMERA_LINE_LENGTH; i++) {
                NXP_TRACEP("idx: %03d\tpix: %03d"
                                   NL, i, image.at(i, ImgType::Thresholded));
                if (currentColor != image.at(i, ImgType::Thresholded)) {
                    regions.at(regions.size() - 1).right = i;
                    regions.emplace_back(nxpbc::Region({i, i, image.at(i, ImgType::Thresholded)}));
                }
                currentColor = static_cast<uint8_t>(image.at(i, ImgType::Thresholded));
            }
            regions.at(regions.size() - 1).right = CAMERA_LINE_LENGTH - 1;
            regions.at(0).left = 0;

            biggestWhiteRegion = regions.at(0);

            for (nxpbc::Region &r : regions) {
                if ((r.color == COLOR_WHITE) && (r.getSize() > biggestWhiteRegion.getSize())) {
                    biggestWhiteRegion = r;
                }
            }

            NXP_TRACEP("Nalezena nejvetsi bila oblast: Levy index: %d, Pravy index: %d, Stred %d, Velikost %d"
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
        return {left, right};
    }

    unsigned int LineTracer::getRight() const {
        return static_cast<unsigned int>(imageRegionList_.back().second.right);
    }

    unsigned int LineTracer::getLeft() const {
        return static_cast<unsigned int>(imageRegionList_.back().second.left);
    }

}
