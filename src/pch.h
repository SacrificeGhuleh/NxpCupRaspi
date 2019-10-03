//
// Created by Richard Zvonek on 30-Nov-18.
//
#ifndef NXP_PCH_H
#define NXP_PCH_H

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>
#include <list>
#include <string>
#include <utility>
#include <vector>



#if defined(WIN32) || defined(__linux__)
#define millis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#endif

#ifdef WIN32
  
//  #include <tfc.h>
  #include <fstream>
  #include <iostream>
  #include <sstream>
#include <ctime>
  
  #define NL "\n"
  
  #ifdef QT_BUILD
    #include <QtWidgets>
    #include <QtWidgets/QApplication>
    #include <opencv2/imgproc.hpp>
    #include <opencv2/highgui.hpp>
  #endif
#endif


#ifdef __MCUXPRESSO
  #include "tfc.h"
  #include "tfc_k6xf.h"
  #include "board.h"
  #include "peripherals.h"
  #include "pin_mux.h"
  #include "clock_config.h"
  #include "MK66F18.h"
  #include "fsl_debug_console.h"
  #ifndef NL
    #define NL "\r\n"
  #endif
  #define printf DbgConsole_Printf

#endif
#endif //NXP_PCH_H
