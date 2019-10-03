//
// Created by Richard Zvonek on 21-Nov-18.
//

#ifndef NXPCUPRASPI_IMAGETYPE_H
#define NXPCUPRASPI_IMAGETYPE_H

namespace nxpbc {
  
  /**
   * @brief Výčtový typ pro určení obrazových dat
   */
  enum class ImgType {
    /**
     * @brief Čistá, nezpracovaná data z kamery
     */
        Raw = 0,
    
    /**
     * @brief Normalizovaná obrazová data
     */
        Normalized,
    
    /**
     * @brief Obrazová data oříznutá prahováním
     */
        Thresholded
  };
  
}

#endif //NXPCUPRASPI_IMAGETYPE_H
