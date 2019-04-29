//
// Created by zvone on 26-Feb-19.
//

#if defined(__MCUXPRESSO) || 1

#include "pch.h"
#include "NxpImageAbstract.h"
#include "NxpImageFreescale.h"
#include "ImageType.h"
#include "Logger.h"
#include "NxpDefines.h"

#define PRINT_ORIGINAL      0
#define PRINT_BLUR          0
#define PRINT_CUT           0
#define PRINT_NORM          0
#define PRINT_THRESH        0

#define USE_MEDIAN_FILTER   0


namespace nxpbc {

    NxpImage::NxpImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) : NxpImageAbstract() {


#if PRINT_ORIGINAL
        NXP_TRACE("Original img" NL);
        NxpImageAbstract::printImg(rawImage);
#endif

#if USE_MEDIAN_FILTER
        uint16_t blurImage[CAMERA_LINE_LENGTH] = {0};
        fastMedianBlur(rawImage, blurImage, 1);
        std::memcpy(rawImage, blurImage, sizeof(rawImage));
#endif

#if PRINT_BLUR
        NXP_TRACE("Blur img" NL);
        NxpImageAbstract::printImg(rawImage);
#endif

        computeMinMax(rawImage);
        if (diversity_ < LOW_DIVERSITY) {
            for (int i = 0; i < CAMERA_LINE_LENGTH; i++) {
                thresholdedImage_[i] = COLOR_WHITE;
                rawImage[i] = COLOR_WHITE_ORIGINAL;
            }
        } else {
            cut(rawImage);

#if PRINT_CUT
            NXP_TRACE("Cut img" NL);
        NxpImageAbstract::printImg(rawImage);
#endif

            normalize(rawImage, thresholdedImage_);

#if PRINT_NORM
            NXP_TRACE("Normalized img" NL);
        NxpImageAbstract::printImg(thresholdedImage_);
#endif

            threshValue_ = getAverageThreshold(thresholdedImage_);
            threshold(thresholdedImage_, thresholdedImage_);

#if PRINT_THRESH
            NXP_TRACE("Thresh img" NL);
        NxpImageAbstract::printImg(thresholdedImage_);
#endif
        }

    }

    uint16_t NxpImage::at(uint8_t index, ImgType type) const {
        if (index >= 128) {
            NXP_ERROR("NXP image index mimo pole"
                              NL);
            return 0;
        }
        switch (type) {
            case ImgType::Thresholded:return thresholdedImage_[index];
            default:return 0;
        }
    }

    void NxpImage::printImg(ImgType type) {
        switch (type) {
            case ImgType::Thresholded:NxpImageAbstract::printImg(thresholdedImage_);
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
