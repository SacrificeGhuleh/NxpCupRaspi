//
// Created by zvone on 26-Feb-19.
//

#ifndef NXPCUPRASPI_NXPIMAGERASPI_H
#define NXPCUPRASPI_NXPIMAGERASPI_H


#if defined(__linux__) || defined(WIN32)

#include "NxpImageAbstract.h"

namespace nxpbc {
    /**
     * @brief Třída pro uložení obrazových dat pro Raspberry Pi
     */
    class NxpImage : public NxpImageAbstract {
    public:
        NxpImage();

        /**
         * @brief Konstruktor
         * @param rawImage Obraz z kamery
         */
        NxpImage(uint16_t(&img)[CAMERA_LINE_LENGTH]);

        /**
          * @brief Getter pro vybrany pixel
          * @param index index pixelu
          * @param type typ obrazu, ze kterého se má pixel vzít
          * @return pixel
          */
        virtual uint16_t at(uint8_t index, ImgType type) const override;

        /**
         * @brief Funkce pro výpis obrazu
         * @param type Typ obrazu
         */
        virtual void printImg(ImgType type) override;

    protected:
        //virtual void printImg(uint16_t (&img)[CAMERA_LINE_LENGTH]) override;

        /**
         * @brief Funkce pro kompletní zpracování obrazu
         */
        virtual void process() override;

    private:
        /**
         * @brief Setter pro přidání obrazu z kamery
         * @param rawImage
         */
        void setRawImage(uint16_t (&rawImage)[CAMERA_LINE_LENGTH]);

        /**
         * @brief Originální obraz
         */
        uint16_t rawImage_[CAMERA_LINE_LENGTH];

        /**
         * @brief NOrmaliovaný obraz
         */
        uint16_t normalizedImage_[CAMERA_LINE_LENGTH];

    };
}
#endif
#endif //NXPCUPRASPI_NXPIMAGERASPI_H
