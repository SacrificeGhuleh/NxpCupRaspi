//
// Created by zvone on 21-Feb-19.
//

#ifndef NXPCUPRASPI_NXPCARFREESCALE_H
#define NXPCUPRASPI_NXPCARFREESCALE_H


#if defined(__MCUXPRESSO) || 1


#include "NxpCarAbstract.h"

//class Enet;

namespace nxpbc {
    class NxpCar : public NxpCarAbstract {
    private:
        void setSendData();
    protected:
        virtual void handleBtns(unsigned char buttons) override;

    public:
        NxpCar();

        virtual ~NxpCar();

        //Enet *enet;

        void update() override;
    };


};

#endif

#endif //NXPCUPRASPI_NXPCARFREESCALE_H
