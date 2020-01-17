#ifndef DRIVELOGIC
#define DRIVELOGIC

#include <Arduino.h>
#include <Servo.h>

#define MOTOR_ENABLE 5
#define MOTOR_INPUTA 16
#define MOTOR_INPUTB 17
#define SERVO_INPUT 18


// in this implementation all angles are processed in degrees

class Actor {
    public:
    Actor();
    void drive(bool);
    void halt();
    void setSteering(int);
    void steer(int);

    private:
    int _enable;
    int _inputA;
    int _inputB;
    Servo _servo;
    int _steerRotation;
};


class DriveLogic {
    public:
    DriveLogic();
    void forward();
    void backward();
    void stop();
    void rotate(int);

    void reactOnSensors(int*);

    private:
    int _mode;
    int _globalRotation;
    Actor _actor;
};

#endif //DRIVELOGIC