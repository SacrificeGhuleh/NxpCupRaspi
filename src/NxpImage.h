//
// Created by zvone on 28-Feb-19.
//

#ifndef NXPCUPRASPI_NXPIMAGE_H
#define NXPCUPRASPI_NXPIMAGE_H

#if defined(__linux__) || defined(WIN32)

    #include "NxpImageRaspi.h"
#endif
#if defined(__MCUXPRESSO)

    #include "NxpImageFreescale.h"
#endif


#endif //NXPCUPRASPI_NXPIMAGE_H
