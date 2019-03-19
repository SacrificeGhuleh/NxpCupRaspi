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

        uint8_t right, left;
//
//        bool foundLeft = false;
//        bool foundRight = false;
//        uint8_t newLeftIndex = 0;
//        uint8_t newRightIndex = 0;

//
//        if (hasPrevDistance) {
//            /*
//             * OPRAVIT
//             * BUG
//             * */
//
//#if defined(__linux__) || defined(WIN32)
//            Region previousRegion = imageRegionList_.back().second;
//#endif
//#if defined(__MCUXPRESSO)
//            Region previousRegion = imageRegionList_.back();
//#endif
//            int prevLeftIndex = previousRegion.left;
//            int prevRightIndex = previousRegion.right;
//            int counter = 0;
//            int i = 0;
//            while (std::abs(i) < PREV_LINE_SEARCH_REGION) {
//                counter++;
//                /*
//                 * Ochrana proti zacykleni
//                 * */
//                if (counter - 1 > PREV_LINE_SEARCH_REGION * 2) {
//                    break;
//                }
//
//                /*NXP_TRACEP("Hledam podle predchozi vzdalenosti, i: %d"
//                                   NL, i);*/
//                if (foundLeft && foundRight) {
//                    break;
//                }
//
//                if (!foundLeft) {
//                    newLeftIndex = static_cast<uint8_t>(prevLeftIndex + i);
//                }
//
//                if (!foundRight) {
//                    newRightIndex = static_cast<uint8_t>(prevRightIndex + i);
//                }
//
//                if (newLeftIndex <= 0 ||
//                    newLeftIndex >= CAMERA_LINE_LENGTH - 1 ||
//                    newRightIndex <= 0 ||
//                    newRightIndex >= CAMERA_LINE_LENGTH - 1) {
//                    continue;
//                }
//
//                if (!foundLeft) {
//                    uint16_t leftLeftColor = image.atThresh(
//                            newLeftIndex,
//                            ImgType::Thresholded);
//                    uint16_t rightLeftColor = image.atThresh(
//                            static_cast<const uint8_t>(newLeftIndex + 1),
//                            ImgType::Thresholded);
//
//                    if (leftLeftColor == COLOR_BLACK && rightLeftColor == COLOR_WHITE) {
//                        NXP_TRACEP("Nalezen levy index: %d"
//                                           NL, newLeftIndex);
//                        foundLeft = true;
//                    }
//                }
//
//                if (!foundRight) {
//                    uint16_t leftRightColor = image.atThresh(
//                            static_cast<const uint8_t>(newRightIndex - 1),
//                            ImgType::Thresholded);
//
//                    uint16_t rightRightColor = image.atThresh(
//                            newRightIndex,
//                            ImgType::Thresholded);
//
//                    if (rightRightColor == COLOR_BLACK && leftRightColor == COLOR_WHITE) {
//                        NXP_TRACEP("Nalezen pravy index: %d"
//                                           NL, newRightIndex);
//                        foundRight = true;
//                    }
//                }
//
//                i *= -1;
//                if (i >= 0) {
//                    i++;
//                }
//            }
//        }
        if (hasPrevDistance) {
            Region previousRegion;
            if (findByPreviousIndex(image, previousRegion)) {
                regions.emplace_back(previousRegion);
                return previousRegion;

            }
        }


//        if (foundLeft && foundRight) {
//            regions.emplace_back(nxpbc::Region(newLeftIndex, newRightIndex, COLOR_WHITE));
//        }


        NXP_TRACE("Hledam indexy podle regionu"
                          NL);
        uint8_t currentColor = static_cast<uint8_t>(image.atThresh(Region::minLeft));
        regions.emplace_back(nxpbc::Region({Region::minLeft, Region::minLeft, currentColor}));
        for (uint8_t i = Region::minLeft; i <= Region::maxRight; i++) {
            /*NXP_TRACEP("idx: %03d\tpix: %03d"
                               NL, i, image.atThresh(i, ImgType::Thresholded));*/
            if (currentColor != image.atThresh(i)) {
                if (regions.size() > 30) {
                    NXP_WARN("Dojde ti pamet"
                                     NL);
                    break;
                }
                regions.at(regions.size() - 1).right = i;
                regions.emplace_back(nxpbc::Region({i, i, image.atThresh(i)}));
            }
            currentColor = static_cast<uint8_t>(image.atThresh(i));

        }
        regions.at(regions.size() - 1).right = Region::maxRight;
        regions.at(0).left = Region::minLeft;

        //biggestWhiteRegion = regions.at(0);
        biggestWhiteRegion = {Region::minLeft, Region::minLeft, COLOR_WHITE};

        uint8_t whiteRegionsCount = 0;
        uint8_t blackRegionsCount = 0;

        for (nxpbc::Region &r : regions) {
            if (r.isWhite()) {
                whiteRegionsCount++;
                if ((r.getSize() > biggestWhiteRegion.getSize())) {
                    biggestWhiteRegion = r;
                }
            } else {
                blackRegionsCount++;
            }
        }

        NXP_INFOP("Pocet nalezenych oblasti: \tBILE: %d\tCERNE: %d"
                          NL, whiteRegionsCount, blackRegionsCount);

        NXP_INFOP("Nejvetsi bila oblast: Levy index: %d, Pravy index: %d, Stred %d, Velikost %d"
                          NL,
                  biggestWhiteRegion.left,
                  biggestWhiteRegion.right,
                  biggestWhiteRegion.getCenter(),
                  biggestWhiteRegion.getSize());

        left = static_cast<uint8_t>(biggestWhiteRegion.left);
        right = static_cast<uint8_t>(biggestWhiteRegion.right);

//        left = newLeftIndex;
//        right = newRightIndex;

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
            NXP_TRACEP("Region: %s velikost: %03d, stred: %03d, left: %03d, right: %03d"
                               NL,
                       region.color == COLOR_BLACK ? "BLACK" : "WHITE",
                       region.getSize(),
                       region.getCenter(),
                       region.left,
                       region.right);
        }
        return {left, right, COLOR_WHITE};
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

    bool LineTracer::findByPreviousIndex(const NxpImage &image, Region &r) {

        //bool foundLeft = false;
        //bool foundRight = false;
        uint8_t newLeftIndex = 0;
        uint8_t newRightIndex = 0;
        uint8_t status = 0b00000000;

#define foundLeftBitIndex   5
#define foundRightBitIndex  4

#define isFoundLeft()       bitRead(status, foundLeftBitIndex)
#define setFoundLeft(val)   bitWrite(status, foundLeftBitIndex ,val)

#define isFoundRight()      bitRead(status, foundRightBitIndex)
#define setFoundRight(val)  bitWrite(status, foundRightBitIndex ,val)

#define outOfBounds(index)  (index <= BLACK_COUNT) || (index >= CAMERA_LINE_LENGTH - BLACK_COUNT - 1)

        /**
         * TODO
         *  status bity
         *  0000 0000
         *
         *  Nalezena cara vlevo     = 5 = 32
         *  Nalezena cara vpravo    = 4 = 16
         *  Vpravo napravo nehledat = 3 = 8
         *  Vpravo nalevo  nehledat = 2 = 4
         *  Vlevo  napravo nehledat = 1 = 2
         *  Vlevo  nalevo  nehledat = 0 = 1
         *
         *
         */

#if defined(__linux__) || defined(WIN32)
        Region previousRegion = imageRegionList_.back().second;
#endif
#if defined(__MCUXPRESSO)
        Region previousRegion = imageRegionList_.back();
#endif

        uint8_t prevLeftIndex = previousRegion.left;
        uint8_t prevRightIndex = previousRegion.right;
        uint8_t counter = 0;
        int8_t i = 0;
        while (std::abs(i) < PREV_LINE_SEARCH_REGION) {
            counter++;
            /*
             * Ochrana proti zacykleni
             * */
            if (counter - 1 > PREV_LINE_SEARCH_REGION * 2) {
                break;
            }

            if (isFoundLeft() && isFoundRight()) {
                r.left = newLeftIndex;
                r.right = newRightIndex;
                break;
            }

            if (!isFoundLeft()) {
                newLeftIndex = static_cast<uint8_t>(prevLeftIndex + i);
            }

            if (!isFoundRight()) {
                newRightIndex = static_cast<uint8_t>(prevRightIndex + i);
            }

            if (outOfBounds(newLeftIndex) || outOfBounds(newRightIndex)) {
                continue;
            }

            if (!isFoundLeft()) {
                uint8_t leftLeftColor = image.atThresh(
                        newLeftIndex);
                uint8_t rightLeftColor = image.atThresh(
                        newLeftIndex + 1);

                if (leftLeftColor == COLOR_BLACK && rightLeftColor == COLOR_WHITE) {
                    NXP_TRACEP("Nalezen levy index: %d"
                                       NL, newLeftIndex);
                    setFoundLeft(true);
                }
            }

            if (!isFoundRight()) {
                uint8_t leftRightColor = image.atThresh(
                        static_cast<const uint8_t>(newRightIndex - 1));

                uint8_t rightRightColor = image.atThresh(
                        newRightIndex);

                if (rightRightColor == COLOR_BLACK && leftRightColor == COLOR_WHITE) {
                    NXP_TRACEP("Nalezen pravy index: %d"
                                       NL, newRightIndex);
                    setFoundRight(true);
                }
            }

            i *= -1;
            if (i >= 0) {
                i++;
            }
        }

        return isFoundLeft() && isFoundRight();
    }

}
