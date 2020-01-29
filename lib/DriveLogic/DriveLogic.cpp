#include "DriveLogic.h"
#include "DebugServer.h"

const int freq = 5000;
const int ledChannel = 2;
const int resolution = 10; 

int sign(int x) {
    return (x > 0) - (x < 0);
}

DriveLogic::DriveLogic() {
    _globalRotation = 0;
    _debug = false;
}

void DriveLogic::activateDebug() {
    _debug = true;
}

void DriveLogic::deactivateDebug() {
    _debug = false;
}


void DriveLogic::forward() {
    if(_debug) 
        wPrintln("Driving forward...");
    _actor.drive(true);
}

void DriveLogic::backward() {
    if(_debug) 
        wPrintln("Driving backward...");
    _actor.drive(false);
}

void DriveLogic::stop() {
    if(_debug) 
        wPrintln("stopping...");
    _actor.halt();
}

void DriveLogic::driveSpiral() {
    if(_debug) 
        wPrintln("Driving spiral...");
    _actor.setSteering(10);
    _actor.drive(true);
}

// TODO: execute a real rotation with the steering wheel
// donno how this works, though...
void DriveLogic::rotate(int angle) {
    // _actor.setSteering(angle);
    _currentlyRotating = true;
    _rotationAngle = angle;
    _startRotationAngle = angle;
    _rotationStage = 0;
}

void DriveLogic::setActorSteering(int angle) {
    if(_debug) 
        wPrintln("steering angle: "+String(angle)+"...");
    _actor.setSteering(angle);
}

void DriveLogic::setActorSpeed(int speed) {
    if(_debug) 
        wPrintln("setting speed: "+String(speed)+"...");
    _actor.setSpeed(speed);
}

// TODO: abort first steering if angle is too small
void DriveLogic::_rotateStep() {
    // execute a step of the current rotation
    int rotationDir = sign(_rotationAngle);
    uint64_t duration = millis() - _prevRotationStep;
    _rotationAngle = abs(_rotationAngle) - duration * abs(_actor.steerAngle() - 90.0) * DEGREES_PER_MILLISECOND_PER_STEER_DEGREE;
    _rotationAngle *= rotationDir;
    if(_rotationStage == 0) {
        _actor.steer(rotationDir * MAX_STEER_WHEEL_TURN_SPEED);
        if((rotationDir == 1 && _actor.steerAngle() >= 175) || (rotationDir == -1 && _actor.steerAngle() <= 5)) {
            _rotationStage = 1;
            _angleAfterSpeedUp = _startRotationAngle - _rotationAngle;
        }
    }
    else if(_rotationStage == 1) {
        if(_rotationAngle <= 3.0 * _angleAfterSpeedUp)
            _rotationStage = 2;
    }
    else if(_rotationStage == 2) {
        _actor.steer(-rotationDir * MAX_STEER_WHEEL_TURN_SPEED);
        if(_actor.steerAngle() == 90) {
            _rotationStage = 3;
        }
    }
    else if(_rotationStage == 3) {
        // _actor.steer(-rotationDir * MAX_STEER_WHEEL_TURN_SPEED);
        // if()
    }
    _prevRotationStep = millis();
}

void DriveLogic::reactOnSensors(int* sensorValues) {
    _sensors = sensorValues;
    if(_currentlyRotating) {
        _rotateStep();
        return;
    }

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

    // digitalWrite(_enable, HIGH);
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(_enable, ledChannel);
    ledcWrite(ledChannel, 1024);
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

byte Actor::steerAngle() {
    return _steerRotation;
}

void Actor::setSpeed(int speed) {
    ledcWrite(ledChannel, speed);
}

void Actor::setSteering(int angle) {
    _servo.write(angle);
    _steerRotation = angle;
}

void Actor::steer(int angleDelta) {
    setSteering(_steerRotation + angleDelta);
}