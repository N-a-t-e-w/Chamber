/*
   MyStepper is used to add more specific functionality to the stepper motor

   MyStepper() requires &stepper and int sleepPin

   isCalibrated() returns true if stepper has been calibrated


   calibrateStepper() requires homeSensor and AwaySensor to be set.  Runs stepper CW to open chamber and runs until the _awaySensor is triggered, the _awayPosition is saved.
                      The stepper then runs CCW closing the chamber, and runs until the _homeSensor is triggered, this position is saved as _homePosition

   goHome() moves the stepper to the _homePosition and stops when either the position is reached or the _homeSensor is triggered

   goAway() moves the stepper to the _awayPosition and stops when either the position is reached or the _awaySesnor is triggered

   goTo() moves the stepper pos steps (+ CW or -CCW), DOES NOT STOP FOR NOTHING BE CAREFUL


*/



#include "MyStepper.h"
#include <Arduino.h>
#include <AccelStepper.h>


AccelStepper *_stepper;

MyStepper::MyStepper(AccelStepper *stepper, int sleepPin) {
  _stepper = stepper;
  _sleepPin = sleepPin;
}

//returns true if stepper has been calibrated
bool MyStepper::isCalibrated() {
  return _calibrated;
}


//calibrates stepper, sets positions for home and away based on when it reached the sensor
String MyStepper::calibrateStepper() {
  digitalWrite(_sleepPin, HIGH);
  delay(500);
  _stepper->setMaxSpeed(350);
  _stepper->setAcceleration(225);

  if (_homeSet & _awaySet) {
    _stepper->move(10000);
    while (!digitalRead(_awaySensor) == LOW) {
      _stepper->run();
    }
    _awayPosition = _stepper->currentPosition();
    _stepper->setCurrentPosition(_awayPosition); //stops the motor without deceleration
    delay(500);
    _stepper->move(-10000);
    while (!digitalRead(_homeSensor) == LOW) {
      _stepper->run();
    }
    _homePosition = _stepper->currentPosition();
    _stepper->setCurrentPosition(_homePosition); //stops the motor without deceleration

    _calibrated = true;
    digitalWrite(_sleepPin, LOW);
    delay(500);
    return "CALIBRATION SUCCESFUL";
  } else {
    return "Please set HOME and AWAY";
  }


}

//moves to position and stops based off position or either sensor
void MyStepper::goTo(int pos) {
  _stepper->setMaxSpeed(350);
  _stepper->setAcceleration(200);
  digitalWrite(_sleepPin, HIGH);
  _stepper->move(pos);
  while (!_stepper->distanceToGo() == 0 ) {
    _stepper->run();
  }

}

//moves to position and stops based on sensor or position
String MyStepper::goHome() {
  if (_calibrated) {
    _stepper->moveTo(_homePosition);
    for (int i = 0; i < 50; i++) {
      _stepper->run();
    }
    while (!digitalRead(_homeSensor) == LOW & !_stepper->distanceToGo() == 0) {
      _stepper->run();
    }
    _stepper->setCurrentPosition(_homePosition); //stops without needing deceleration
    return "Moved Home";
  } else {
    return "Please Calibrate";
  }

}

//moves to position and stops based on sensor or position
String MyStepper::goAway() {
  if (_calibrated) {
    _stepper->moveTo(_awayPosition);
    for (int i = 0; i < 50; i++) {
      _stepper->run();
    }
    while (!digitalRead(_awaySensor) == LOW & !_stepper->distanceToGo() == 0) {
      _stepper->run();
    }
    _stepper->setCurrentPosition(_awayPosition); //stops without needing deceleration
    return "Moved Away";
  } else {
    return "Please Calibrate";
  }
}

void MyStepper::sleep() {
  digitalWrite(_sleepPin, LOW);
  delay(500);
}

void MyStepper::wake() {
  digitalWrite(_sleepPin, HIGH);
  delay(500);
}

//home sensor, where it can reside without load
void MyStepper::setHomeSensor(int pin) {
  _homeSensor = pin;
  _homeSet = true;
}

//away sensor
void MyStepper::setAwaySensor(int pin) {
  _awaySensor = pin;
  _awaySet = true;
}

void MyStepper::setSensor(int pin1, int pin2) {
  _sensor1 = pin1;
  _sensor2 = pin2;
}
