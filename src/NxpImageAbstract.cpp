//
// Created by Richard Zvonek on 21-Nov-18.
//

#include "pch.h"
#include <deque>
#include "NxpImageAbstract.h"
#include "NxpDefines.h"
#include "ImageType.h"
#include "Logger.h"
#include "NxpMath.h"

namespace nxpbc {

    /*NxpImageAbstract::NxpImageAbstract(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) :
            thresholdedImage_{0},
            max_{COLOR_BLACK},
            min_{COLOR_WHITE} {
    }*/

    NxpImageAbstract::NxpImageAbstract() :
            thresholdedImage_{0},
            max_{COLOR_BLACK},
            min_{COLOR_WHITE} {

    }

    /*void NxpImageAbstract::setRawImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]) {
        std::memcpy(rawImage_, rawImage, sizeof(rawImage));
        process();
    }*/

    void NxpImageAbstract::computeMinMax(const uint16_t (&img)[CAMERA_LINE_LENGTH]) {
        max_ = COLOR_BLACK;
        //min_ = COLOR_WHITE;
        min_ = COLOR_WHITE_ORIGINAL;
        for (int i = BLACK_COUNT; i < CAMERA_LINE_LENGTH - BLACK_COUNT; i++) {
            uint16_t pixel = img[i];
            if (pixel > max_) {
                max_ = pixel;
                /*NXP_TRACEP("max_: %04d"
                                   NL, max_);*/
            }

            if (pixel < min_) {
                min_ = pixel;
                /*NXP_TRACEP("min_: %04d"
                                   NL, min_);*/
            }

            if (max_ == COLOR_WHITE_ORIGINAL/*COLOR_WHITE*/ && min_ == COLOR_BLACK) {
                break;
            }


        }

        NXP_INFOP("Nalezeno:\tMIN:%04d, \tMAX:%04d"
                          NL, min_, max_);
        diversity_ = MAX(max_ , min_)- MIN(min_, max_);
    }

    void NxpImageAbstract::cut(
            uint16_t (&srcImg)[CAMERA_LINE_LENGTH]) {
        for (int col = 0; col < BLACK_COUNT; col++) {
            srcImg[col] = max_;
        }

        for (int col = CAMERA_LINE_LENGTH - 1; col > CAMERA_LINE_LENGTH - 1 - BLACK_COUNT; col--) {
            srcImg[col] = max_;
        }

    }

    void NxpImageAbstract::normalize(
            const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            uint16_t (&dstImg)[CAMERA_LINE_LENGTH]) {
        for (int i = 0; i < CAMERA_LINE_LENGTH; i++) {
            float pixel = static_cast<float>(srcImg[i]);
            pixel -= min_;
            pixel *= COLOR_WHITE;
            pixel /= (max_ - min_);
            dstImg[i] = static_cast<uint16_t >(pixel);
        }
    }

    uint16_t NxpImageAbstract::getAverageThreshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH]) {
        long sum = 0;
        for (int i = BLACK_COUNT; i < CAMERA_LINE_LENGTH - BLACK_COUNT; i++) {
            sum += srcImg[i];
        }
        auto avg = static_cast<uint16_t>(sum / CAMERA_LINE_LENGTH - (2 * BLACK_COUNT));
        return avg;
    }

    void
    NxpImageAbstract::threshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH]) {
        for (int i = 0; i < CAMERA_LINE_LENGTH; i++) {
            if (srcImg[i] < threshValue_)
                dstImg[i] = COLOR_BLACK;
            else
                dstImg[i] = COLOR_WHITE;
        }
    }

    void NxpImageAbstract::fastMedianBlur(
            uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            const int pixels) {


        const int buffSize = pixels * 2 + 1;
        std::vector<uint16_t> blurBuffer;

        for (int i = pixels; i < CAMERA_LINE_LENGTH - pixels; i += buffSize) {
            for (int j = -pixels; j <= pixels; j++) {
                blurBuffer.emplace_back(srcImg[i + j]);
            }

            for (int j = -pixels; j <= pixels; j++) {
                srcImg[i + j] = median(blurBuffer);
            }
            blurBuffer.clear();
        }

    }

    void NxpImageAbstract::fastMedianBlur(
            const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
            const int pixels) {
        std::memcpy(dstImg, srcImg, CAMERA_LINE_LENGTH);

        const int buffSize = pixels * 2 + 1;
        std::vector<uint16_t> blurBuffer;

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

    void NxpImageAbstract::slowMedianBlur(
            const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
            const int pixels) {
        std::memcpy(dstImg, srcImg, CAMERA_LINE_LENGTH);
        std::vector<uint16_t> blurBuffer;

        for (int i = pixels; i < CAMERA_LINE_LENGTH - pixels; i++) {
            for (int j = -pixels; j <= pixels; j++) {
                blurBuffer.emplace_back(srcImg[i - j]);
            }
            std::sort(blurBuffer.begin(), blurBuffer.end());
            dstImg[i] = blurBuffer.at(pixels + 1);
            blurBuffer.clear();
        }
    }

    void NxpImageAbstract::printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]) {
        for (uint16_t pix : img) {
            printf("%d|", pix);
        }
        printf(NL);
    }


    void NxpImageAbstract::slowMedianBlur(
            uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
            int pixels) {
        const int buffSize = pixels * 2 + 1;
        //std::vector<uint16_t> blurBuffer;
        std::deque<uint16_t> pixBuffer;

        for (int i = 0; i < CAMERA_LINE_LENGTH; i++) {
            //for (int i = pixels; i < CAMERA_LINE_LENGTH - pixels; i += buffSize) {
            if (pixBuffer.size() < pixels) {
                pixBuffer.emplace_back(srcImg[i]);
                continue;
            }
            pixBuffer.emplace_back(srcImg[i]);
            //blurBuffer = std::vector<uint16_t>(pixBuffer.begin(), pixBuffer.end());

            for (int j = -pixels; j <= pixels; j++) {
                srcImg[i + j] = median<uint16_t>(std::vector<uint16_t>(pixBuffer.begin(), pixBuffer.end()));
            }
            //blurBuffer.clear();
            pixBuffer.pop_front();
        }
    }

    uint8_t NxpImageAbstract::atThresh(uint8_t index) const {
        return thresholdedImage_[index];
    }

    uint16_t NxpImageAbstract::getMin_() const {
        return min_;
    }

    uint16_t NxpImageAbstract::getMax_() const {
        return max_;
    }

    uint16_t NxpImageAbstract::getThreshValue_() const {
        return threshValue_;
    }

    uint8_t NxpImageAbstract::getDiversity_() const {
        return diversity_;
    }
/*
    uint16_t NxpImageAbstract::at(const uint16_t index, ImgType type) const {
        if (index >= CAMERA_LINE_LENGTH) {
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

    void NxpImageAbstract::printImg(ImgType type) {
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

    void NxpImageAbstract::printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]) {
        for (uint16_t pix : img) {
            printf("%d|", pix);
        }
        printf("" NL);
    }
*/
}
