//
// Created by zvone on 26-Feb-19.
//

#if defined(__MCUXPRESSO) || 1

#include "pch.h"
#include "NxpImageAbstract.h"
#include "NxpImageFreescale.h"
#include "ImageType.h"
#include "Logger.h"

namespace nxpbc {

    NxpImage::NxpImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) : NxpImageAbstract() {
        uint16_t blurImage[CAMERA_LINE_LENGTH] = {0};
        //NXP_TRACE("Original img" NL);
        //NxpImageAbstract::printImg(rawImage);
        slowMedianBlur(rawImage, blurImage, 3);
        std::memcpy(rawImage, blurImage, sizeof(rawImage));

        //NXP_TRACE("Blur img" NL);
        //NxpImageAbstract::printImg(rawImage);
        computeMinMax(rawImage);
        cut(rawImage);
        //NXP_TRACE("Cut img" NL);
        //NxpImageAbstract::printImg(rawImage);
        normalize(rawImage, thresholdedImage_);
        //NXP_TRACE("Normalized img" NL);
        //NxpImageAbstract::printImg(thresholdedImage_);
        threshValue_ = getAverageThreshold(thresholdedImage_);
        threshold(thresholdedImage_, thresholdedImage_);
        //NXP_TRACE("Thresh img" NL);
        //NxpImageAbstract::printImg(thresholdedImage_);
    }

    uint16_t NxpImage::at(uint8_t index, ImgType type) const {
        if (index >= 128) {
            NXP_ERROR("NXP image index mimo pole"
                              NL);
            return 0;
        }
        switch (type) {
            case ImgType::Thresholded:
                return thresholdedImage_[index];
            default:
                return 0;
        }
    }

    void NxpImage::printImg(ImgType type) {
        switch (type) {
            case ImgType::Thresholded:
                NxpImageAbstract::printImg(thresholdedImage_);
                break;
        }
    }

    void NxpImage::process() {
        /*NXP_TRACE("Original img" NL);
    	NxpImageAbstract::printImg(rawImage);
        computeMinMax(rawImage);
    	cut(rawImage);
    	NXP_TRACE("Cut img" NL);
    	NxpImageAbstract::printImg(rawImage);
        normalize(rawImage, thresholdedImage_);
    	NXP_TRACE("Normalized img" NL);
    	NxpImageAbstract::printImg(thresholdedImage_);
        threshValue_ = getAverageThreshold(thresholdedImage_);
        threshold(thresholdedImage_, thresholdedImage_);
    	NXP_TRACE("Thresh img" NL);
    	NxpImageAbstract::printImg(thresholdedImage_);*/
    }
}
#endif
