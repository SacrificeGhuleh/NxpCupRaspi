//
// Created by zvone on 26-Feb-19.
//

#ifndef NXPCUPRASPI_NXPIMAGERASPI_H
#define NXPCUPRASPI_NXPIMAGERASPI_H


#if defined(__linux__) || defined(WIN32)

#include "NxpImageAbstract.h"

namespace nxpbc {
    class NxpImage : public NxpImageAbstract {
    public:
        NxpImage();

        NxpImage(uint16_t(&img)[CAMERA_LINE_LENGTH]);

        virtual uint16_t at(uint8_t index, ImgType type) const override;

        virtual void printImg(ImgType type) override;

    protected:
        //virtual void printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]) override;

        virtual void process() override;

    private:
        void setRawImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]);

        uint16_t rawImage_[CAMERA_LINE_LENGTH];
        uint16_t normalizedImage_[CAMERA_LINE_LENGTH];

    };
}
#endif
#endif //NXPCUPRASPI_NXPIMAGERASPI_H
