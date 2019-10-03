//
// Created by zvone on 23-Mar-19.
//

#ifndef NXPCUPRASPI_NXPMATH_H
#define NXPCUPRASPI_NXPMATH_H

#include <vector>
#include <cstdint>
#include <algorithm>

namespace nxpbc {
  
  /**
   * @brief Funkce pro výpočet mediánu obecného pole
   * @tparam T číselný datový typ
   * @param v vektor hodnot
   * @return Medián hodnot vektoru
   */
  template<class T>
  T median(std::vector<T> v) {
    size_t n = v.size() / 2;
    std::nth_element(v.begin(), v.begin() + n, v.end());
    int vn = v[n];
    if (v.size() % 2 == 1) {
      return vn;
    } else {
      std::nth_element(v.begin(), v.begin() + n - 1, v.end());
      return 0.5 * (vn + v[n - 1]);
    }
  }
}


#endif //NXPCUPRASPI_NXPMATH_H
