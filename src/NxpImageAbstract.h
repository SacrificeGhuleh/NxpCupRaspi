//
// Created by Richard Zvonek on 21-Nov-18.
//

#ifndef NXPCUPRASPI_NXPIMAGEABSTRACT_H
#define NXPCUPRASPI_NXPIMAGEABSTRACT_H

#ifndef TFC_CAMERA_LINE_LENGTH
#define TFC_CAMERA_LINE_LENGTH 128
#endif

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


namespace nxpbc {
  
  enum class ImgType;
  
  /**
   * @brief Abstraktní třída pro uložení obrazových dat
   */
  class NxpImageAbstract {
  public:
    NxpImageAbstract();
    //NxpImageAbstract(uint16_t (&rawImage)[TFC_CAMERA_LINE_LENGTH]);
    
    /**
     * @brief Metoda pro přístup k pixelům obrazu
     * @param index Inde pixelu
     * @param type Typ záznamu
     * @return Hodnota pixelu
     */
    virtual uint16_t at(uint8_t index, ImgType type) const = 0;
    
    /**
     * @brief Metoda pro přístup k pixelům thresholdovaného obrazu
     * @param index Index pixelu
     * @return Hodnota pixelu
     */
    virtual uint8_t atThresh(uint8_t index) const;
    
    /**
     * @brief Metoda pro výpis obrazu
     * @param type Typ obrazu
     */
    virtual void printImg(ImgType type) = 0;
    
    /**
     * @brief Metoda pro obecný výpis obrazu
     * @param img obrazová data
     */
    static void printImg(uint16_t (&img)[TFC_CAMERA_LINE_LENGTH]);
  
  protected :
    /**
     * @brief Prahovaný obraz
     */
    uint16_t thresholdedImage_[TFC_CAMERA_LINE_LENGTH];
    //uint16_t thresholdedImage_[TFC_CAMERA_LINE_LENGTH];
    
    /**
     * @brief Minimální pixel v obraze
     */
    uint16_t min_;
    
    
    /**
     * @brief Maximální pixel v obraze
     */
    uint16_t max_;
    
    /**
     * @brief Prahovací hodnota
     */
    uint16_t threshValue_;
  public:
    
    
    /**
     * @brief Getter pro minimální pixel v obraze
     * @return Minimum
     */
    uint16_t getMin_() const;
    
    /**
     * @brief Getter pro maximální pixel v obraze
     * @return Maximum
     */
    uint16_t getMax_() const;
    
    /**
     * @brief Getter pro prahovací hodnotu
     * @return Prahovací hodnota
     */
    uint16_t getThreshValue_() const;
    
    /**
     * @brief Getter pro diverzitu
     * @return Diverzita obrazu
     */
    int16_t getDiversity_() const;
    
    /**
     * @brief Funkce pro zjištění, jestli je obraz jednobarevný
     * @return True, pokud je nízká diverzita
     */
    bool isLowDiversity() const;
  
  protected:
    
    /**
     * @brief Diverzita obrazu
     */
    int16_t diversity_;
    
    /**
     * @brief Funkce pro kompletní zpracování obrazu
     */
    virtual void process() = 0;
    
    
    /**
     * @brief Metoda pro získán minima a maxima
     * @param img vstup
     */
    virtual void computeMinMax(const uint16_t (&img)[TFC_CAMERA_LINE_LENGTH]);
    
    
    /**
     * @brief Metoda pro ořez nepotřebných pixelů
     * @param img vstup
     */
    virtual void cut(uint16_t (&img)[TFC_CAMERA_LINE_LENGTH]);
    
    
    /**
     * @brief Metoda pro normalizaci obrazu
     * @param srcImg vstup
     * @param dstImg výstup
     */
    virtual void normalize(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
                           uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH]);
    
    /**
     * @brief Metoda pro výpočet prahovací hodnoty
     * @param srcImg vstup
     * @return prahovací hodnota
     */
    virtual uint16_t getAverageThreshold(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH]);
    
    /**
     * @brief Metoda pro prahování
     * @param srcImg Vstup
     * @param dstImg Výstup
     */
    virtual void threshold(
        const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
        uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH]);
    
    /**
     * @brief Mediánový filtr
     * @param srcImg Vstup
     * @param dstImg Výstup
     * @param pixels velikost mediánového filtru
     */
    virtual void slowMedianBlur(
        const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
        uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH],
        int pixels);
    
    /**
     * @brief Mediánový filtr
     * @param srcImg Obrazová data
     * @param pixels velikost mediánového filtru
     */
    virtual void slowMedianBlur(
        uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
        int pixels);
    
    /**
     * @brief Mediánový filtr
     * @param srcImg Vstup
     * @param dstImg Výstup
     * @param pixels velikost mediánového filtru
     */
    virtual void fastMedianBlur(
        const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
        uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH],
        int pixels);
    
    /**
     * @brief Mediánový filtr
     * @param srcImg Obrazová data
     * @param pixels velikost mediánového filtru
     */
    virtual void fastMedianBlur(
        uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
        int pixels);
    
    
  };
}
#endif //NXPCUPRASPI_NXPIMAGEABSTRACT_H
