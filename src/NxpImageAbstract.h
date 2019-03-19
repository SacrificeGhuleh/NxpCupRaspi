//
// Created by Richard Zvonek on 21-Nov-18.
//

#ifndef NXPCUPRASPI_NXPIMAGEABSTRACT_H
#define NXPCUPRASPI_NXPIMAGEABSTRACT_H

#ifndef CAMERA_LINE_LENGTH
#define CAMERA_LINE_LENGTH 128
#endif

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


namespace nxpbc {

    enum class ImgType;

    class NxpImageAbstract {
    public:
        NxpImageAbstract();
        //NxpImageAbstract(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]);

        virtual uint16_t at(uint8_t index, ImgType type) const = 0;

        virtual uint8_t atThresh(uint8_t index) const;

        virtual void printImg(ImgType type) = 0;

        static void printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]);

    protected :
        uint16_t thresholdedImage_[CAMERA_LINE_LENGTH];
        //uint16_t thresholdedImage_[CAMERA_LINE_LENGTH];
        uint16_t min_;
        uint16_t max_;
        uint16_t threshValue_;


        virtual void process() = 0;

        virtual void computeMinMax(const uint16_t (&img)[CAMERA_LINE_LENGTH]);

        virtual void cut(uint16_t (&img)[CAMERA_LINE_LENGTH]);

        virtual void normalize(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
                               uint16_t (&dstImg)[CAMERA_LINE_LENGTH]);

        virtual uint16_t getAverageThreshold(const uint16_t (&srcImg)[CAMERA_LINE_LENGTH]);

        virtual void threshold(
                const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
                uint16_t (&dstImg)[CAMERA_LINE_LENGTH]);

        virtual void slowMedianBlur(
                const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
                uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
                int pixels);

        virtual void slowMedianBlur(
                uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
                int pixels);

        virtual void fastMedianBlur(
                const uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
                uint16_t (&dstImg)[CAMERA_LINE_LENGTH],
                int pixels);

        virtual void fastMedianBlur(
                uint16_t (&srcImg)[CAMERA_LINE_LENGTH],
                int pixels);

        template<class T>
        T median(std::vector<T> v);

    };
}
#endif //NXPCUPRASPI_NXPIMAGEABSTRACT_H
