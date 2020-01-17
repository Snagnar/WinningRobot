#include "DriveLogic.h"
#include "DebugServer.h"

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

void DriveLogic::driveSpiral() {
    _actor.setSteering(10);
    _actor.drive(true);
}

void DriveLogic::rotate(int angle) {
    _actor.setSteering(angle);
}


void DriveLogic::reactOnSensors(int* sensorValues) {
    _sensors = sensorValues;

    int diffNum = _getDifferencesOfOppositeSensors();
    if(_mode == 0) { // this is when we never crossed a line before
        if(diffNum <2)
            driveSpiral();
        else
            _mode = 1;
    }
    else if(_mode == 1) {
        forward();
        if(diffNum < 2)
            _justSet = false;
        else if(!_justSet || diffNum > 2) {
            int newRotation = _getRotationFromLineCrossing(diffNum);
            if(newRotation <= 60 || newRotation >= 300) {
                // alert("not serious");
                _rotationsQueue.push_front((_globalRotation+newRotation) % 360);
                if(_rotationsQueue.size() > 20) 
                    _rotationsQueue.pop_back();
                int avg = 0;
                for(auto it : _rotationsQueue) {
                    // avg += Math.pow(2, 10-x)*rotationsQueue[x];
                    // count += Math.pow(2, 10-x);
                    avg += it;
                }
                wPrintln("avg: "+(avg/_rotationsQueue.size()));
                newRotation = ((avg / _rotationsQueue.size()) - _globalRotation) % 360;
            }
            else
                _rotationsQueue.clear();

            
            _globalRotation += newRotation;
            rotate(newRotation);
            _justSet = true;
        }
    }
}

int DriveLogic::_getDifferencesOfOppositeSensors() {
    int c = 0;
    for(int x = 0; x<3; x++)
        if(_sensors[x]!=_sensors[(x+3) % 6])
            c++;
    return c;
}

int DriveLogic::_getRotationFromLineCrossing(int diffNum) {
    int angle = 0, 
        color1 = 0, 
        color2 = 0;

    if(diffNum != 2 && diffNum != 3)
        return 0;

    for(int x = 0; x<6; x++) {
        int prev = _sensors[(x-1) % 6],
            own = _sensors[x],
            next = _sensors[(x + 1) % 6],
            nextNext = _sensors[(x + 2) % 6];
        
        if( diffNum == 2 &&
            own == next && prev != own && nextNext != own) {
                
                angle =  x * 60 + 30; // actually the angle is x * 36 degrees -> to radians: x * 36 * pi / 180
                color1 = own;
                color2 = _sensors[(x + 3) % 6];
                break;
        }
        if( diffNum == 3 &&
            own == next && own == prev) {

                angle = x * 60; // actually the angle is x * 36 degrees -> to radians: x * 36 * pi / 180
                color1 = own;
                color2 = _sensors[(x + 3) % 6];
                break;
        }
    }
    if(((color2 + 1) % 3) == color1)
        return angle % 360;
    return (180 + angle) % 360;
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