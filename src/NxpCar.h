//
// Created by zvone on 28-Feb-19.
//

#ifndef NXPCUPRASPI_NXPCAR_H
#define NXPCUPRASPI_NXPCAR_H

#if defined(__linux__) || defined(WIN32)

    #include "NxpCarRaspi.h"

#endif
#if defined(__MCUXPRESSO)

    #include "NxpCarFreescale.h"
#endif

#endif //NXPCUPRASPI_NXPCAR_H
