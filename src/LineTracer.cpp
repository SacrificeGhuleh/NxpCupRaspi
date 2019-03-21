//
// Created by Richard Zvonek on 21-Nov-18.
//

#include "pch.h"

#include "LineTracer.h"
#include "Settings.h"
#include "Logger.h"
#include "ImageType.h"

namespace nxpbc {


#define TOLERANCE 1

#define REGION_STANDARD_SIZE 85
#define REGION_SIZE_COEFICIENT 0.5
#define REGION_COMPUTED_SIZE (REGION_STANDARD_SIZE*REGION_SIZE_COEFICIENT)
#define REGION_DISTANCE 30

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

        //std::vector<nxpbc::Region> currentRegions_;
        currentRegions_.clear();
        Region biggestWhiteRegion;

        uint8_t right, left;
        bool regionByPreviousIndexFound = false;
        if (hasPrevDistance) {
            //Region previousRegion;
            regionByPreviousIndexFound = findByPreviousIndex(image, biggestWhiteRegion);
            if (regionByPreviousIndexFound) {
                currentRegions_.emplace_back(biggestWhiteRegion);
                computedRegion_ = false;
                return biggestWhiteRegion;
            }
        }

        NXP_TRACE("Hledam indexy podle regionu"
                          NL);
        uint8_t currentColor = static_cast<uint8_t>(image.atThresh(Region::minLeft));
        currentRegions_.emplace_back(nxpbc::Region({Region::minLeft, Region::minLeft, currentColor}));


        uint8_t searchLeftIdx;
        uint8_t searchRightIdx;

        if (regionByPreviousIndexFound) {
            searchLeftIdx = biggestWhiteRegion.left;
            searchRightIdx = biggestWhiteRegion.right;
        } else {
            //searchLeftIdx = Region::minLeft;
            //searchRightIdx = Region::maxRight;
            searchLeftIdx = 0;
            searchRightIdx = CAMERA_LINE_LENGTH - 1;
        }

        for (uint8_t i = searchLeftIdx; i <= searchRightIdx; i++) {
            /*NXP_TRACEP("idx: %03d\tpix: %03d"
                               NL, i, image.atThresh(i, ImgType::Thresholded));*/
            if (currentColor != image.atThresh(i)) {
                if (currentRegions_.size() > MAX_REGIONS_COUNT) {
                    NXP_WARN("Nalezen vysoky pocet oblasti, konec hledani."
                                     NL);
                    break;
                }
                currentRegions_.at(currentRegions_.size() - 1).right = i;
                currentRegions_.emplace_back(nxpbc::Region({i, i, image.atThresh(i)}));
            }
            currentColor = static_cast<uint8_t>(image.atThresh(i));

        }
        currentRegions_.at(currentRegions_.size() - 1).right = Region::maxRight;
        currentRegions_.at(0).left = Region::minLeft;

        //biggestWhiteRegion = currentRegions_.at(0);
        if (!regionByPreviousIndexFound) {
            biggestWhiteRegion = {Region::minLeft, Region::minLeft, COLOR_WHITE};
        }

        uint8_t whiteRegionsCount = 0;
        uint8_t blackRegionsCount = 0;

        for (Region &r : currentRegions_) {
            if (r.isWhite()) {  //if white
                whiteRegionsCount++;
                if ((r.getSize() > biggestWhiteRegion.getSize())) {
                    biggestWhiteRegion = r;
                }
            } else {            //if black
                blackRegionsCount++;
            }
        }


        /*
         * pokud nasel jen jednu caru
         * */
        computedRegion_ = false;
        bool hasLeftLine = false;
        bool hasRightLine = false;
        for (Region &r : currentRegions_) {
            if (r.isBlack()) {
                if (r.getCenter() < biggestWhiteRegion.getCenter()) {
                    hasLeftLine = true;
                } else if (r.getCenter() > biggestWhiteRegion.getCenter()) {
                    hasRightLine = true;
                }
            }

        }

        if (!(hasLeftLine && hasRightLine)) {
            computedRegion_ = true;
            if (biggestWhiteRegion.getCenter() > CAMERA_LINE_LENGTH / 2) {
                biggestWhiteRegion.left = biggestWhiteRegion.left;
                biggestWhiteRegion.right = biggestWhiteRegion.left + REGION_DISTANCE + REGION_COMPUTED_SIZE;
            } else {
                biggestWhiteRegion.left = biggestWhiteRegion.right - REGION_DISTANCE - REGION_COMPUTED_SIZE;
                biggestWhiteRegion.right = biggestWhiteRegion.right;

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


        for (Region &region : currentRegions_) {
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


    unsigned int LineTracer::getRight() {
        std::vector<unsigned int> rightLines;
        rightLines.reserve(imageRegionList_.size());


        unsigned int sum = 0;
        for (auto idx : imageRegionList_) {
#if defined(__linux__) || defined(WIN32)
            rightLines.emplace_back(idx.second.right);
            sum += idx.second.right;
#endif
#if defined(__MCUXPRESSO)

            rightLines.emplace_back(idx.right);
        sum += idx.right;
#endif
        }

        /*
         * Median
         * */
        std::sort(rightLines.begin(), rightLines.end());

        unsigned int current = rightLines.at(rightLines.size() - 1);
        unsigned int median = rightLines.at(rightLines.size() / 2);

        unsigned int average = sum / rightLines.size();

        //unchangedRight_ = ((current >= median - TOLERANCE) && (current <= median + TOLERANCE));
        unchangedRight_ = ((current >= average - TOLERANCE) && (current <= average + TOLERANCE));

        //return median;
        return average;
    }

    unsigned int LineTracer::getLeft() {
        std::vector<unsigned int> leftLines;
        leftLines.reserve(imageRegionList_.size());


        unsigned int sum = 0;
        for (auto idx : imageRegionList_) {
#if defined(__linux__) || defined(WIN32)
            leftLines.emplace_back(idx.second.left);
            sum += idx.second.left;
#endif
#if defined(__MCUXPRESSO)

            leftLines.emplace_back(idx.left);
        sum += idx.left;
#endif
        }

        /*
         * Median
         * */
        std::sort(leftLines.begin(), leftLines.end());

        unsigned int current = leftLines.at(leftLines.size() - 1);
        unsigned int median = leftLines.at(leftLines.size() / 2);

        unsigned int average = sum / leftLines.size();

        //unchangedLeft_ = ((current >= median - TOLERANCE) && (current <= median + TOLERANCE));
        unchangedLeft_ = ((current >= average - TOLERANCE) && (current <= average + TOLERANCE));

        //return median;
        return average;
    }

    bool LineTracer::findByPreviousIndex(const NxpImage &image, Region &r) {

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
         *  Nalezena cara vlevo     = 5 = 32 -- OK
         *  Nalezena cara vpravo    = 4 = 16 -- OK
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

    void LineTracer::reset() {
        this->imageRegionList_.clear();


    }

}
