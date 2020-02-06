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
    _actor.drive(FORWARD);
}

void DriveLogic::backward() {
    if(_debug) 
        wPrintln("Driving backward...");
    _actor.drive(BACKWARD);
}

void DriveLogic::stop() {
    if(_debug) 
        wPrintln("stopping...");
    if(_actor.drivingDirection() != HALT) {
        _actor.drive(_actor.drivingDirection() * -1);
        
        delay(50);
    }

    _actor.halt();
}

void DriveLogic::activateSpiral() {
    _spi_interp = 0.0;
    _prevRotationStep = millis();
    _actor.setSpeed(MAX_MOTOR_SPEED);
    forward();
}

void DriveLogic::deactivateSpiral() {
    _actor.setSteering(90);
    _actor.setSpeed(DEFAULT_SPEED);
}

void DriveLogic::driveSpiral() {
    // forward();
    // if(_debug)
    //     wPrintln("interpol: "+String(_spi_interp));
    double duration = millis() - _prevRotationStep;
    _prevRotationStep = millis();
    // if(_actor.steerAngle() > _maxSteerAngle) {
    //     // if(_debug) 
    //     //     wPrintln("Driving spiral..."+String(_actor.steerAngle())+" and time: "+String(duration)+" final: "+String(-duration * _maxSteerWheelTurnPerMS));;

    //     _actor.steer(-duration * _maxSteerWheelTurnPerMS);
    //     _prevRotationStep = millis();
    // }
    if(_spi_interp < 0.25) {
        forward();
        _actor.setSteering(70);
    }
    else if(_spi_interp < 0.5) {
        stop();
        _actor.setSteering(90);
    }
    else if(_spi_interp < 0.75) {
        backward();
        _actor.setSteering(110);
    }
    else {
        stop();
        _actor.setSteering(90);
    }
    _spi_interp += duration/1700.0;
    _spi_interp = fmod(_spi_interp, 1.0);
}

// TODO: execute a real rotation with the steering wheel
// donno how this works, though...
void DriveLogic::rotate(int angle) {

    if(_debug)
        wPrintln("rotating angle: "+String(angle));
    if(!angle) {
        if(_debug)
            wPrintln("no rotation required!");
        return;
    }
    _currentlyRotating = true;
    _rotationAngle = (double)angle;
    _startRotationAngle = (double)angle;
    _rotationStage = 0;
    _prevRotationStep = millis();
    _angleAfterSpeedUp = 0.0;
    _prevActorSpeed = _actor.speed();
    _actor.setSpeed(MAX_MOTOR_SPEED);
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
    // if(_debug)
    //     wPrintln("in rotationStep");
    // execute a step of the current rotation
    int rotationDir = sign(_rotationAngle);
    uint64_t duration = millis() - _prevRotationStep;
    byte currAngle = _actor.steerAngle();
    _rotationAngle = abs(_rotationAngle) - duration * abs(currAngle - 90) * _dpmpsd;
    _rotationAngle *= rotationDir;
    double angleTraveled = _startRotationAngle - _rotationAngle;
    _globalRotation += sign(currAngle - 90) * duration * abs(currAngle - 90) * _dpmpsd;
    if(_debug)
    wPrintln("rotationAngle: "+String(_rotationAngle) + " and stage: "+String(_rotationStage) + " lastTraveled: "+String(sign(currAngle - 90) *duration * abs(_actor.steerAngle() - 90.0) * _dpmpsd)+" grot: "+String(_globalRotation));
    if(_rotationStage == 0) {
        _actor.steer(rotationDir * _maxSteerWheelTurnPerMS * duration);
        if(angleTraveled > _rotationAngle) {
            if(_debug)
                wPrintln("abort in s1");
            _rotationStage = 2;
        }
        if((rotationDir == 1 && currAngle >= (180 - _maxSteerAngle)) || (rotationDir == -1 && currAngle <= _maxSteerAngle)) {
            _rotationStage = 1;
            _angleAfterSpeedUp = angleTraveled;
        }
    }
    else if(_rotationStage == 1) {
        if(abs(_rotationAngle) <=  abs(_angleAfterSpeedUp))
            _rotationStage = 2;
        // if(abs(_rotationAngle) <= 1.0)
        //     _actor.setSteering(90);
    }
    else if(_rotationStage == 2) {
        if(abs(_rotationAngle) <= 0.5) {
            _actor.setSteering(90);
        }
        else {
            if(_debug)
                wPrintln("in rotationStage 2 angle: "+String(currAngle));
            _actor.steer(sign(90.0 - currAngle) * _maxSteerWheelTurnPerMS * duration);
        }
        if(currAngle >=89.0 && currAngle <= 91.0) {
            _currentlyRotating = false;
            _actor.setSpeed(_prevActorSpeed);
        }
    }
    _prevRotationStep = millis();
}

void DriveLogic::setDPMSPSD(double value) {
    _dpmpsd = value;
}

void DriveLogic::setMaxSteerPerMs(double value) {
    _maxSteerWheelTurnPerMS = value;
}

void DriveLogic::setMaxSteerAngle(double value) {
    _maxSteerAngle = value;
}

bool DriveLogic::rotating() {
    return _currentlyRotating;
}

void DriveLogic::performRotationStep() {
    if(_currentlyRotating)
        _rotateStep();
}

void DriveLogic::reactOnSensors(int* sensorValues) {
    _sensors = sensorValues;

    int diffNum = _getDifferencesOfOppositeSensors();
    if(_mode == 0) { // this is when we never crossed a line before
        if(diffNum <2)
            driveSpiral();
        else {
            _mode = 1;
            deactivateSpiral();
        }
    }
    else if(_mode == 1) {
        if(diffNum < 2)
            _justSet = false;
        else if(!_justSet || diffNum > 2) {
            int newRotation = _getRotationFromLineCrossing(diffNum);
            if(newRotation <= 60 || newRotation >= 300) {
                _rotationsQueue.push_front((int)(_globalRotation+newRotation) % 360);
                if(_rotationsQueue.size() > 20) 
                    _rotationsQueue.pop_back();
                int avg = 0;
                for(auto it : _rotationsQueue) {
                    avg += it;
                }
                wPrintln("avg: "+(avg/_rotationsQueue.size()));
                newRotation = (int)((avg / _rotationsQueue.size()) - _globalRotation) % 360;
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
    setSpeed(DEFAULT_SPEED);
}

void Actor::drive(int8_t direction) {
    if(direction == FORWARD) {
        digitalWrite(_inputA, LOW);
        digitalWrite(_inputB, HIGH);
    }
    else if(direction == BACKWARD) {
        digitalWrite(_inputA, HIGH);
        digitalWrite(_inputB, LOW);
    }
    else
        halt();
    _drivingDirection = direction;
}

bool Actor::drivingDirection() {
    return _drivingDirection;
}

void Actor::halt() {
    _drivingDirection = HALT;
    digitalWrite(_inputA, HIGH);
    digitalWrite(_inputB, HIGH);
}

byte Actor::steerAngle() {
    return _steerRotation;
}

void Actor::setSpeed(int speed) {
    ledcWrite(ledChannel, min(speed, MAX_MOTOR_SPEED));
    _speed = speed;
}

int Actor::speed() {
    return _speed;
}

void Actor::setSteering(int angle) {
    _servo.write(angle);
    _steerRotation = angle;
}

void Actor::steer(int angleDelta) {
    setSteering(_steerRotation + angleDelta);
}