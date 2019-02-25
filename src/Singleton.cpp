//
// Created by Richard Zvonek on 14-Nov-18.
//

#include "Singleton.h"

namespace nxpbc {

    Singleton *Singleton::getInstance() {
        static Singleton *instance;
        if (!instance)
            instance = new Singleton();

        return instance;
    }

    Singleton::Singleton() {}

}