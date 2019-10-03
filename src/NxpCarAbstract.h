//
// Created by Richard Zvonek on 01-Dec-18.
//

#ifndef NXPCUPRASPI_NXPCAR_H
#define NXPCUPRASPI_NXPCAR_H

#include "NxpDefines.h"
#include "NxpSendFrame.h"
#include "NxpModeSetting.h"

class TFC;

class PID_new;

//struct SendData;

namespace nxpbc {
  enum class MotorsState;
  
  class LineTracer;
  
  class NxpCommunication;
  
  class PID;
  
  /**
   * @brief Abstraktní třída pro model NXP auta.
   */
  class NxpCarAbstract {
  public:
    
    /**
     * @brief Konstruktor třídy
     */
    NxpCarAbstract();
    
    /**
     * @brief Metoda pro řízení auta v nekonečné smyčce
     */
    virtual void update() = 0;
    
    /**
     * @brief Metoda pro získání aktuálního stavu jízdy autíčka.
     * @return True, pokud se má opakovat metoda update()
     */
    virtual bool isRunning();
    
    virtual ~NxpCarAbstract();
    
    /**
     * @brief Metoda indikující, jestli má proběhnout reset auta
     * @return True, pokud se má auto resetovat
     */
    bool shouldReset();
    
    /**
     * @brief Getter pro objekt třídy TFC
     * @return Objekt třídy TFC
     */
    TFC *getTfc();
    
    /**
     * @brief Data pro odesílání dat přes UDP
     */
    SendData sendData_;
    
    /**
     * @brief Getter pro aktuální stav motorů
     * @return Aktuální stav motorů
     */
    MotorsState getMotorsState();
    
    /**
     * @brief Přepne auto do módu stání, pokud je v módu brždění
     */
    virtual void switchToStayMode();
  
  protected:
    
    /**
     * @brief bity pro indikaci zmeny serva
     * 0b0000 0001  INCREASING - servo zvysuje natoceni
     * 0b0000 0010  DECREASING - servo snizuje natoceni
     * 0b0000 0000  STAYING - servo zachovava pozici
     */
    uint8_t servoIncreasingBits_;
    
    /**
     * @brief Počet nalezených oblastí v předchozím snímku
     */
    uint8_t prevZonesFoundCount_;
    
    /**
     * @brief Proměnná pro uložení informace, jestli je auto v zóně zpomalení
     */
    bool inSpeedCheckZone_;
    
    /**
     * @brief Čítač pro zpoždění detekce speed check zóny
     */
    uint8_t speedCheckZoneDebounce_;
    
    /**
     * @brief Aktuální stav motoru
     */
    MotorsState motorsState_;
    /**
     * @brief Proměnná pro rozlišení, jestli je auto v provozu
     */
    bool running_;
    
    /**
     * @brief Proměnná pro nastavení příznaku, jestli má být auto resetováno
     */
    bool shouldReset_;
    
    /**
     * @brief Základní rychlost motorů
     */
    uint16_t motorSpeed_;
    //int leftSpeed_;
    //int rightSpeed_;
    
    /**
     * @brief Kanál serva
     */
    const bool servoChannel_;
    
    /**
     * @brief Předchozí pozice serva
     */
    int16_t prevServoPos_;
    
    /**
     * @brief Aktuáljní pozice serva
     */
    int16_t servoPos_;
    
    /**
     * @brief Příznak, jestli jsou právě ošetřovány zákmity na tlačítku
     */
    bool debounce_;
    
    /**
     * @brief Čítač pro ošetření zákmitů tlačítka
     */
    uint8_t debounceCounter_;
    
    /**
     * @brief Délka ošetření zákmitů tlačítka
     */
    const uint8_t debounceCounterMax_;
    
    /**
     * @brief hodnoty tlačítek
     */
    uint8_t btns_;
    
    uint16_t timestamp_;
    uint16_t timestampDiff_;
    
    /**
     * @brief Levá vzdálenost čáry
     */
    uint8_t left_;
    
    /**
     * @brief Pravá vzdálenost čáry
     */
    uint8_t right_;
    
    /**
     * @brief Timer pro počítání rozjezdu auta
     */
    int startTimer_;
    
    /**
     * @brief Nastavení serva
     */
    float steerSetting_;
    
    /**
     * @brief Instance třídy pro hledání čar
     */
    LineTracer *tracer_;
    
    PID *pid_;
    
    /**
     * @brief Instance třídy TFC
     */
    TFC *tfc_;
    
    /**
     * @brief Instance regulátoru
     */
    PID_new *steerRegulator_;
    
    /**
     * @brief Vstup pro regulátor
     */
    double steerRegulatorInput_;
    
    /**
     * @brief Výstup z regulátoru
     */
    double steerRegulatorOutput_;
    
    /**
     * @brief Cílová hodnota regulátoru
     */
    double steerRegulatorTarget_;
    
    /**
     * @brief Aktuální nastavený mód
     */
    NxpModeSetting modeSetting_;
    
    /**
     * @brief Metoda pro výpis aktuálního stavu autíčka do konzole.
     */
    virtual void printCurrentState();
    
    /**
     * @brief Metoda pro ovládání tlačítek na autíčku
     * @param buttons Stav tlačítek
     */
    virtual void handleBtns(unsigned char buttons) = 0;
    
    /**
     * @brief Metoda pro předání parametrů jízdy autíčku.
     */
    virtual void setRide();
    
    
    /**
     * @brief Metoda pro ořez poměrů stran.
     * @param ratioDiff Rozdíl poměrů vzdáleností od čáry.
     */
    virtual void clipRatio(float ratioDiff);
    
    /**
     * @brief Metoda pro rozjezd autíčka.
     */
    virtual void start();
    
    /**
     * @brief Metoda pro výpočet natočení serva
     * @return natočení serva ve stupních
     */
    virtual float servoToAngle();
    
    /**
     * @brief Funkce pro reset regulátoru
     */
    virtual void resetRegulator();
    
    /**
     * @brief Funkce pro reset auta
     */
    virtual void handleReset();
    
    
  };
}

#endif //NXPCUPRASPI_NXPCAR_H
