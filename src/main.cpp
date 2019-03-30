//
// Created by Richard Zvonek on 02-Dec-18.
//


#if defined(__linux__) || defined(WIN32)

#include "NxpCarAbstract.h"
#include "NxpCarRaspi.h"

int main() {

    auto *car = new nxpbc::NxpCar();

    while (car->isRunning()) {
        car->update();
    }

    DELETE_AND_PARK(car);

}





#endif