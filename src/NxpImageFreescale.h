//
// Created by zvone on 26-Feb-19.
//

#ifndef NXPCUPRASPI_NXPFREESCALEIMAGE_H
#define NXPCUPRASPI_NXPFREESCALEIMAGE_H

#if defined(__MCUXPRESSO) || 1
//#include "ImageType.h"
#include "NxpImageAbstract.h"

#ifndef TFC_CAMERA_LINE_LENGTH
#define TFC_CAMERA_LINE_LENGTH 128
#endif

namespace nxpbc {

    //class NxpImageAbstract;
    enum class ImgType;
  
  /**
   * @brief Třída pro uložení obrazových dat pro FRDM-K66F
   */
    class NxpImage : public NxpImageAbstract {
    public:
        /**
         * @brief Konstruktor
         * @param rawImage Obraz z kamery
         */
        NxpImage(uint16_t (&rawImage)[TFC_CAMERA_LINE_LENGTH]);

        /**
         * @brief Getter pro vybrany pixel
         * @param index index pixelu
         * @param type typ obrazu, ze kterého se má pixel vzít
         * @return pixel
         */
        virtual uint16_t at(uint8_t index, ImgType type) const;
        /**
         * @brief Funkce pro výpis obrazu
         * @param type Typ obrazu
         */
        virtual void printImg(ImgType type);

    protected:

        /**
         * @brief Funkce pro kompletní zpracování obrazu
         */
        virtual void process() override;

    private:
        //uint16_t thresholdedImage_[TFC_CAMERA_LINE_LENGTH];
        //uint16_t min_;
        //uint16_t max_;
        //uint16_t threshValue_;
    };
}
#endif
#endif //NXPCUPRASPI_NXPFREESCALEIMAGE_H
