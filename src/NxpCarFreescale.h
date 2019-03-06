//
// Created by zvone on 21-Feb-19.
//

#ifndef NXPCUPRASPI_NXPCARFREESCALE_H
#define NXPCUPRASPI_NXPCARFREESCALE_H


#if defined(__MCUXPRESSO)


#include "NxpCarAbstract.h"

namespace nxpbc {
    class NxpCar : public NxpCarAbstract {
    public:
        NxpCar();

        virtual ~NxpCar();

        void update() override;
    };


};

#endif

#endif //NXPCUPRASPI_NXPCARFREESCALE_H
