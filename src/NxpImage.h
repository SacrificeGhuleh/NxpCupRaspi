//
// Created by Richard Zvonek on 21-Nov-18.
//

#ifndef NXPCUPRASPI_NXPIMAGE_H
#define NXPCUPRASPI_NXPIMAGE_H

#ifndef CAMERA_LINE_LENGTH
#define CAMERA_LINE_LENGTH 128
#endif

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

namespace nxpbc {

    enum class ImgType;

    class NxpImage {
    public:
        NxpImage();

        explicit NxpImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]);

        void setRawImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]);

        uint16_t at(const uint8_t index, ImgType type) const;

        void printImg(ImgType type);

    private:

        void printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]);

        uint16_t rawImage_[CAMERA_LINE_LENGTH];
        uint16_t normalizedImage_[CAMERA_LINE_LENGTH];
        uint16_t thresholdedImage_[CAMERA_LINE_LENGTH];
        uint16_t min_;
        uint16_t max_;
        uint16_t threshValue_;

        void computeMinMax(const uint16_t (&img)[CAMERA_LINE_LENGTH]);

        void cut(uint16_t (&img)[CAMERA_LINE_LENGTH]);

        void normalize(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH]);

        uint16_t getAverageThreshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH]);

        void threshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH]);

        void process();

        void slowMedianBlur(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
                            int pixels);

        void fastMedianBlur(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH], uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
                            int pixels);

    };
}
#endif //NXPCUPRASPI_NXPIMAGE_H
