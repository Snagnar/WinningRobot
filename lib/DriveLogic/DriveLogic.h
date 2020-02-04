#ifndef DRIVELOGIC
#define DRIVELOGIC

#include <Arduino.h>
#include <Servo.h>
#include <deque>

#define MOTOR_ENABLE 16
#define MOTOR_INPUTA 17
#define MOTOR_INPUTB 22
#define SERVO_INPUT 13

// this has to be tweaked heavily...
#define DEGREES_PER_MILLISECOND_PER_STEER_DEGREE 0.01
#define MAX_STEER_WHEEL_TURN_SPEED_PER_MILLISECOND 0.1
#define MAX_STEER_ANGLE 40


// in this implementation all angles are processed in degrees

class Actor {
    public:
    Actor();
    void drive(bool);
    void setSpeed(int);
    void halt();
    void setSteering(int);
    void steer(int);
    byte steerAngle();

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
    void rotateStep();
    void driveSpiral();
    void setActorSteering(int);
    void setActorSpeed(int);
    void activateDebug();
    bool rotating();
    void deactivateDebug();
    void performRotationStep();
    void reactOnSensors(int*);
    void setDPMSPSD(double);
    void setMaxSteerPerMs(double);
    void setMaxSteerAngle(double);

    private:
    int _getRotationFromLineCrossing(int);
    int _getDifferencesOfOppositeSensors();
    void _rotateStep();

    uint64_t _prevRotationStep;
    byte _rotationStage;
    byte _rotationAngle;
    byte _angleAfterSpeedUp;
    byte _startRotationAngle;
    double _dpmpsd = DEGREES_PER_MILLISECOND_PER_STEER_DEGREE;
    double _maxSteerWheelTurnPerMS = MAX_STEER_WHEEL_TURN_SPEED_PER_MILLISECOND;
    double _maxSteerAngle = MAX_STEER_ANGLE;

    std::deque<int> _rotationsQueue;
    int _mode;
    bool _currentlyRotating = 0;
    int _globalRotation;
    int* _sensors;
    int _sensorCount = 6;
    bool _justSet = 0;
    bool _debug;
    Actor _actor;
};

#endif //DRIVELOGIC