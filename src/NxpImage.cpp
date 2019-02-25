//
// Created by Richard Zvonek on 21-Nov-18.
//

#include "pch.h"
#include "NxpImage.h"
#include "NxpDefines.h"
#include "ImageType.h"
#include "Logger.h"

namespace nxpbc {

    NxpImage::NxpImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) :
            rawImage_{0},
            normalizedImage_{0},
            thresholdedImage_{0},
            max_{COLOR_BLACK},
            min_{COLOR_WHITE} {
        setRawImage(rawImage);
    }

    NxpImage::NxpImage() :
            rawImage_{0},
            normalizedImage_{0},
            thresholdedImage_{0},
            max_{COLOR_BLACK},
            min_{COLOR_WHITE} {

    }

    void NxpImage::setRawImage(uint16_t (&rawImage)[128]) {
        std::memcpy(rawImage_, rawImage, sizeof(rawImage));
        process();
    }

    void NxpImage::computeMinMax(const uint16_t (&img)[CAMERA_LINE_LENGTH]) {
        max_ = COLOR_BLACK;
        //min_ = COLOR_WHITE;
        min_ = COLOR_WHITE_ORIGINAL;
        for (int i = BLACK_COUNT; i < CAMERA_LINE_LENGTH - BLACK_COUNT; i++) {
            uint16_t pixel = img[i];
            if (pixel > max_) {
                max_ = pixel;
            }

            if (pixel < min_) {
                min_ = pixel;
            }

            if (max_ == COLOR_WHITE_ORIGINAL/*COLOR_WHITE*/ && min_ == COLOR_BLACK) {
                break;
            }
        }
    }

    void NxpImage::cut(uint16_t (&srcImg)[CAMERA_LINE_LENGTH]) {
        for (int col = 0; col < BLACK_COUNT; col++) {
            srcImg[col] = max_;
        }

        for (int col = CAMERA_LINE_LENGTH - 1; col > CAMERA_LINE_LENGTH - 1 - BLACK_COUNT; col--) {
            srcImg[col] = max_;
        }

    }

    void NxpImage::normalize(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH]) {
        for (int i = 0; i < CAMERA_LINE_LENGTH; i++) {
            long elemLong = srcImg[i];
            elemLong -= min_;
            elemLong *= COLOR_WHITE;
            elemLong /= (max_ - min_);
            dstImg[i] = static_cast<uint16_t >(elemLong);

        }
    }

    uint16_t NxpImage::getAverageThreshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH]) {
        long sum = 0;
        for (uint16_t pixel : srcImg) {
            sum += pixel;
        }
        auto avg = static_cast<uint16_t>(sum / CAMERA_LINE_LENGTH);
        return avg;
    }

    void NxpImage::threshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH]) {
        for (int i = 0; i < CAMERA_LINE_LENGTH; i++) {
            if (srcImg[i] < threshValue_)
                dstImg[i] = COLOR_BLACK;
            else
                dstImg[i] = COLOR_WHITE;
        }
    }

    void NxpImage::process() {
        computeMinMax(rawImage_);
        cut(rawImage_);
        normalize(rawImage_, normalizedImage_);
        threshValue_ = getAverageThreshold(normalizedImage_);
        threshold(normalizedImage_, thresholdedImage_);
    }

    void NxpImage::fastMedianBlur(
            const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
            const int pixels) {
        std::memcpy(dstImg, srcImg, CAMERA_LINE_LENGTH);

        const int buffSize = pixels * 2 + 1;
        std::vector<uint8_t> blurBuffer;

        for (int i = pixels; i < CAMERA_LINE_LENGTH - pixels; i += buffSize) {
            for (int j = -pixels; j <= pixels; j++) {
                blurBuffer.emplace_back(srcImg[i + j]);
            }
            std::sort(blurBuffer.begin(), blurBuffer.end());
            for (int j = -pixels; j <= pixels; j++) {
                dstImg[i + j] = blurBuffer.at(pixels + 1);
            }
            blurBuffer.clear();
        }
    }

    void NxpImage::slowMedianBlur(
            const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
            const int pixels) {

        std::memcpy(dstImg, srcImg, CAMERA_LINE_LENGTH);
        //int buffSize = pixels * 2 + 1;
        std::vector<uint16_t> blurBuffer/*(buffSize)*/;

        for (int i = pixels; i < CAMERA_LINE_LENGTH - pixels; i++) {
            for (int j = -pixels; j <= pixels; j++) {
                blurBuffer.emplace_back(srcImg[i - j]);
            }
            std::sort(blurBuffer.begin(), blurBuffer.end());
            dstImg[i] = blurBuffer.at(pixels + 1);
            blurBuffer.clear();
        }
    }

    uint16_t NxpImage::at(const uint8_t index, ImgType type) const {
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

    void NxpImage::printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]) {
        for (uint16_t pix : img) {
            printf("%d|", pix);
        }
        printf("" NL);
    }
}
