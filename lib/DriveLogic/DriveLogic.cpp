#include "DriveLogic.h"

DriveLogic::DriveLogic() {
    _globalRotation = 0;
}

void DriveLogic::forward() {
    _actor.drive(true);
}

void DriveLogic::backward() {
    _actor.drive(false);
}

void DriveLogic::stop() {
    _actor.halt();
}

void DriveLogic::rotate(int angle) {
    _actor.setSteering(angle);
}

Actor::Actor() {
    _enable = MOTOR_ENABLE;
    _inputA = MOTOR_INPUTA;
    _inputB = MOTOR_INPUTB;
    _servo.attach(SERVO_INPUT);

    pinMode(_enable, OUTPUT);
    pinMode(_inputA, OUTPUT);
    pinMode(_inputB, OUTPUT);

    digitalWrite(_enable, HIGH);
    setSteering(90);
}

void Actor::drive(bool direction) {
    if(direction) {
        digitalWrite(_inputA, HIGH);
        digitalWrite(_inputB, LOW);
    }
    else {
        digitalWrite(_inputA, LOW);
        digitalWrite(_inputB, HIGH);
    }
}

void Actor::halt() {
    digitalWrite(_inputA, HIGH);
    digitalWrite(_inputB, HIGH);
}

void Actor::setSteering(int angle) {
    _servo.write(angle);
    _steerRotation = angle;
}

void Actor::steer(int angleDelta) {
    setSteering(_steerRotation + angleDelta);
}