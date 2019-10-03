//
// Created by Richard Zvonek on 14-Nov-18.
//

#ifndef NXPCUPRASPI_SINGLETON_H
#define NXPCUPRASPI_SINGLETON_H
namespace nxpbc {
  
  /**
   * @brief Obecna trida pro singleton
   */
  class Singleton {
  public:
    /**
     * @brief Metoda pro ziskani instance tridy Singleton
     * @return Instance tridy Singleton
     */
    static Singleton *getInstance();
  
  protected:
    Singleton();
    
    virtual ~Singleton() = default;
  };
}
#endif //NXPCUPRASPI_SINGLETON_H
