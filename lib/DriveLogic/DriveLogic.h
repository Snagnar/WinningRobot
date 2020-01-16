#ifndef DRIVELOGIC
#define DRIVELOGIC

#include <Arduino.h>

class DriveLogic {
    DriveLogic(int*, int*);
    void forward();
    void rotate(double);

    private:
    int enableA;
    int enableB;
    // int 
};

#endif //DRIVELOGIC