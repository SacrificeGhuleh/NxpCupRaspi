//
// Created by Richard Zvonek on 21-Nov-18.
//

#include "pch.h"
#include "LineTracer.h"
#include "Logger.h"
#include "ImageType.h"
#include "NxpMath.h"
#include "NxpImageFreescale.h"

namespace nxpbc {


#define TOLERANCE 3

#define REGION_STANDARD_SIZE 85
#define REGION_SIZE_COEFICIENT 0.3
#define REGION_COMPUTED_SIZE (REGION_STANDARD_SIZE*REGION_SIZE_COEFICIENT)
#define REGION_DISTANCE 30
  
  LineTracer::LineTracer(const int listSize) :
      listSize_{listSize},
      blackRegionsCount_{0},
      whiteRegionsCount_{0},
      hasLeft_{false},
      hasRight_{false} {
  }
  
  void LineTracer::addImage(const NxpImage &image, bool forceSearchRegions) {
    bool hasPrevLine = true;
    if (imageRegionList_.size() > listSize_) {
      imageRegionList_.pop_front();
    }
    
    if (imageRegionList_.empty())
      hasPrevLine = false;
    
    imageRegionList_.emplace_back(getDistances(image, hasPrevLine, forceSearchRegions));
  }
  
  Region LineTracer::getDistances(const NxpImage &image, bool hasPrevDistance, bool forceSearchRegions) {
    currentRegions_.clear();
    Region biggestWhiteRegion;
    computedRegion_ = false;
    uint8_t right, left;
    
    if (image.isLowDiversity()) {
      whiteRegionsCount_ = 1;
      blackRegionsCount_ = 0;
      
      biggestWhiteRegion.color = COLOR_WHITE;
      biggestWhiteRegion.left = 0;
      biggestWhiteRegion.right = TFC_CAMERA_LINE_LENGTH;
      
      currentRegions_.emplace_back(biggestWhiteRegion);
      return biggestWhiteRegion;
    }
    
    bool regionByPreviousIndexFound = false;
    if (hasPrevDistance) {
      //Region previousRegion;
      regionByPreviousIndexFound = findByPreviousIndex(image, biggestWhiteRegion);
      if (regionByPreviousIndexFound) {
        currentRegions_.emplace_back(biggestWhiteRegion);
        //computedRegion_ = false;
        whiteRegionsCount_ = 1;
        blackRegionsCount_ = 2;
        if (!forceSearchRegions)
          return biggestWhiteRegion;
      }
    }
    
    
    uint8_t searchLeftIdx;
    uint8_t searchRightIdx;
    
    if (regionByPreviousIndexFound) {
      searchLeftIdx = biggestWhiteRegion.left + 1;
      searchRightIdx = biggestWhiteRegion.right - 1;
    } else {
      searchLeftIdx = Region::minLeft;
      searchRightIdx = Region::maxRight;
      //searchLeftIdx = 0;
      //searchRightIdx = TFC_CAMERA_LINE_LENGTH - 1;
    }
    
    
    getRegions(image, searchLeftIdx, searchRightIdx);
    if (regionByPreviousIndexFound) {
      currentRegions_.at(currentRegions_.size() - 1).right = biggestWhiteRegion.right;
      currentRegions_.at(0).left = biggestWhiteRegion.left;
    } else {
      currentRegions_.at(currentRegions_.size() - 1).right = TFC_CAMERA_LINE_LENGTH - 1;
      currentRegions_.at(0).left = 0;
      
    }
    //biggestWhiteRegion = currentRegions_.at(0);
    if (!regionByPreviousIndexFound) {
      biggestWhiteRegion = {Region::minLeft, Region::minLeft, COLOR_WHITE};
    }
    
    whiteRegionsCount_ = 0;
    blackRegionsCount_ = 0;
    
    for (Region &r : currentRegions_) {
      if (r.isWhite()) {  //if white
        whiteRegionsCount_++;
        if ((r.getSize() > biggestWhiteRegion.getSize())) {
          biggestWhiteRegion = r;
        }
      } else {            //if black
        blackRegionsCount_++;
      }
    }
    
    
    /*
     * pokud nasel jen jednu caru
     * */
    //computedRegion_ = false;
    bool hasLeftLine = false;
    bool hasRightLine = false;
    for (Region &r : currentRegions_) {
      if (r.isBlack()) {
        if (r.getCenter() < biggestWhiteRegion.left) {
          hasLeftLine = true;
        } else if (r.getCenter() > biggestWhiteRegion.right) {
          hasRightLine = true;
        }
      }
      
    }
    
    if (!(hasLeftLine && hasRightLine)) {
      computedRegion_ = true;
      
      if (biggestWhiteRegion.getCenter() > TFC_CAMERA_LINE_LENGTH / 2) {
        biggestWhiteRegion.left = biggestWhiteRegion.right - REGION_DISTANCE - REGION_COMPUTED_SIZE;
        biggestWhiteRegion.right = biggestWhiteRegion.right;
      } else {
        biggestWhiteRegion.left = biggestWhiteRegion.left;
        biggestWhiteRegion.right = biggestWhiteRegion.left + REGION_DISTANCE + REGION_COMPUTED_SIZE;
        
        
      }
    }
    
    NXP_INFOP("Pocet nalezenych oblasti: \tBILE: %d\tCERNE: %d"
                  NL, whiteRegionsCount_, blackRegionsCount_);
    
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
    hasLeft_ = hasLeftLine;
    hasRight_ = hasRightLine;
    return {left, right, COLOR_WHITE};
  }
  
  
  unsigned int LineTracer::getRight() {
    std::vector<unsigned int> rightLines;
    rightLines.reserve(imageRegionList_.size());
    
    
    unsigned int sum = 0;
    for (auto idx : imageRegionList_) {
      rightLines.emplace_back(idx.right);
      sum += idx.right;
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
      leftLines.emplace_back(idx.left);
      sum += idx.left;
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

#define outOfBounds(index)  (index <= BLACK_COUNT) || (index >= TFC_CAMERA_LINE_LENGTH - BLACK_COUNT - 1)
    
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
    
    Region previousRegion = imageRegionList_.back();
    
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
  
  std::vector<Region>
  LineTracer::getRegions(const NxpImage &image, uint8_t searchLeftIdx, uint8_t searchRightIdx, bool saveToClass) {
    NXP_TRACE("Hledam indexy podle regionu"
                  NL);
    std::vector<nxpbc::Region> foundRegions;
    
    uint8_t currentColor = static_cast<uint8_t>(image.atThresh(searchLeftIdx));
    foundRegions.emplace_back(Region({searchLeftIdx, searchLeftIdx, currentColor}));
    
    for (uint8_t i = searchLeftIdx; i <= searchRightIdx; i++) {
      /*NXP_TRACEP("idx: %03d\tpix: %03d"
                         NL, i, image.atThresh(i, ImgType::Thresholded));*/
      if (currentColor != image.atThresh(i)) {
        if (foundRegions.size() > MAX_REGIONS_COUNT) {
          NXP_WARN("Nalezen vysoky pocet oblasti, konec hledani."
                       NL);
          break;
        }
        foundRegions.at(foundRegions.size() - 1).right = i;
        foundRegions.emplace_back(nxpbc::Region({i, i, image.atThresh(i)}));
      }
      currentColor = static_cast<uint8_t>(image.atThresh(i));
    }
    
    if (saveToClass) {
      currentRegions_ = foundRegions;
    }
    
    return foundRegions;
  }
  
  std::pair<uint8_t, uint8_t> LineTracer::getDistancesPair() {
    std::pair<uint8_t, uint8_t> currentDistances = {0, 0};
    
    std::pair<uint16_t, uint16_t> sums = {0, 0};
    
    /*
     * Kvuli medianu dve pole
     * */
    std::vector<uint8_t> leftDistances;
    std::vector<uint8_t> rightDistances;
    
    leftDistances.reserve(imageRegionList_.size());
    rightDistances.reserve(imageRegionList_.size());
    
    for (auto idx : imageRegionList_) {
      leftDistances.emplace_back(idx.left);
      rightDistances.emplace_back(idx.right);
      
      sums.first += idx.left;
      sums.second += idx.right;
    }
    
    currentAverage_.first = sums.first / imageRegionList_.size();
    currentAverage_.second = sums.second / imageRegionList_.size();
    
    currentMedian_.first = median<uint8_t>(leftDistances);
    currentMedian_.second = median<uint8_t>(rightDistances);
    
    
    unchangedLeft_ = ((imageRegionList_.back().left >= currentAverage_.first - TOLERANCE) &&
                      (imageRegionList_.back().left <= currentAverage_.first + TOLERANCE));
    unchangedRight_ = ((imageRegionList_.back().right >= currentAverage_.second - TOLERANCE) &&
                       (imageRegionList_.back().right <= currentAverage_.second + TOLERANCE));
    
    currentDistances.first = imageRegionList_.back().left;
    currentDistances.second = imageRegionList_.back().right;
    
    return currentAverage_;
    //return currentDistances;
  }
  
  size_t LineTracer::getListSize() {
    return imageRegionList_.size();
  }
  
  
  bool LineTracer::hasLeft() {
    return hasLeft_;
  }
  
  bool LineTracer::hasRight() {
    return hasRight_;
  }
  
  Region LineTracer::getLastRawRegion() {
    return imageRegionList_.back();
  }
  
}
