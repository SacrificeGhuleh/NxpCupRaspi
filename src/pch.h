//
// Created by Richard Zvonek on 30-Nov-18.
//

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>


#define millis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

#ifdef WIN32

#include <tfc.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define NL "\n"
#ifdef _MSC_VER
#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#endif
#endif

#ifdef __linux__
#define UDP 1
#include <wiringPi.h>
#include <wiringSerial.h>
#include <tfc.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define NL "\n"
#endif

#ifdef __MCUXPRESSO
#include "tfc.h"
#include "tfc_k6xf.h"
#include "fsl_debug_console.h"
#define NL "\r\n"

#define printf DbgConsole_Printf
#endif
