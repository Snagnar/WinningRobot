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
#define DEGREES_PER_MILLISECOND_PER_STEER_DEGREE 0.00242
#define MAX_STEER_WHEEL_TURN_SPEED_PER_MILLISECOND 0.3
#define MAX_STEER_ANGLE 40

#define FORWARD 1
#define BACKWARD -1
#define HALT 0

#define DEFAULT_SPEED 120
#define MAX_MOTOR_SPEED 1024

// in this implementation all angles are processed in degrees

class Actor {
    public:
    Actor();
    void drive(int8_t);
    void setSpeed(int);
    void halt();
    void setSteering(int);
    void steer(int);
    byte steerAngle();
    int speed();
    bool drivingDirection();

    private:
    int _enable;
    int _inputA;
    int _inputB;
    Servo _servo;
    int _steerRotation;
    int _speed;
    bool _drivingDirection;
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
    void activateSpiral();
    void deactivateSpiral();
    void setMaxSteerPerMs(double);
    void setMaxSteerAngle(double);

    private:
    int _getRotationFromLineCrossing(int);
    int _getDifferencesOfOppositeSensors();
    void _rotateStep();

    uint64_t _prevRotationStep;
    byte _rotationStage;
    double _rotationAngle;
    double _angleAfterSpeedUp;
    double _startRotationAngle;
    int _prevActorSpeed;
    double _dpmpsd = DEGREES_PER_MILLISECOND_PER_STEER_DEGREE;
    double _maxSteerWheelTurnPerMS = MAX_STEER_WHEEL_TURN_SPEED_PER_MILLISECOND;
    double _maxSteerAngle = MAX_STEER_ANGLE;

    std::deque<int> _rotationsQueue;
    int _mode;
    bool _currentlyRotating = 0;
    double _globalRotation;
    int* _sensors;
    int _sensorCount = 6;
    bool _justSet = 0;
    bool _debug;
    double _spi_interp = 0.0;
    Actor _actor;
};

#endif //DRIVELOGIC