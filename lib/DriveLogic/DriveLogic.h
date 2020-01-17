#ifndef DRIVELOGIC
#define DRIVELOGIC

#include <Arduino.h>
#include <Servo.h>
#include <deque>

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
    void driveSpiral();

    void reactOnSensors(int*);

    private:
    int _getRotationFromLineCrossing(int);
    int _getDifferencesOfOppositeSensors();

    std::deque<int> _rotationsQueue;
    int _mode;
    int _globalRotation;
    int* _sensors;
    int _sensorCount = 6;
    bool _justSet = 0;
    Actor _actor;
};

#endif //DRIVELOGIC