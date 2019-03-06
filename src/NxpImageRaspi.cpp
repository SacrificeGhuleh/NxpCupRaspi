//
// Created by zvone on 26-Feb-19.
//

#if defined(__linux__) || defined(WIN32)

#include "pch.h"
#include "NxpImageRaspi.h"
#include "Logger.h"
#include "ImageType.h"


namespace nxpbc {
    NxpImage::NxpImage()
            : rawImage_{0},
              normalizedImage_{0} {}

    NxpImage::NxpImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) : NxpImageAbstract() {
        setRawImage(rawImage);
    }

    uint16_t NxpImage::at(uint8_t index, ImgType type) const {
        if (index >= 128) {
            NXP_ERROR("NXP image index mimo pole"
                              NL);
            return 0;
        }
        switch (type) {
            case ImgType::Raw:
                return rawImage_[index];
            case ImgType::Normalized:
                return normalizedImage_[index];
            case ImgType::Thresholded:
                return thresholdedImage_[index];
            default:
                return 0;
        }
    }

    void NxpImage::printImg(ImgType type) {
        switch (type) {
            case ImgType::Raw:
                printImg(rawImage_);
                break;
            case ImgType::Normalized:
                printImg(normalizedImage_);
                break;
            case ImgType::Thresholded:
                printImg(thresholdedImage_);
                break;
        }
    }

/*
    void NxpImage::printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]) {
        for (uint16_t pix : img) {
            printf("%d|", pix);
        }
        printf("" NL);
    }*/

    void NxpImage::process() {
        computeMinMax(rawImage_);
        cut(rawImage_);
        normalize(rawImage_, normalizedImage_);
        threshValue_ = getAverageThreshold(normalizedImage_);
        threshold(normalizedImage_, thresholdedImage_);
    }

    void NxpImage::setRawImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) {
        std::memcpy(rawImage_, rawImage, sizeof(rawImage));
        process();
    }

}


#endif
