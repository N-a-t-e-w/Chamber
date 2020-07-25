#ifndef MyStepper_H
#define MyStepper_H
#include <AccelStepper.h>
#include "Arduino.h"

class MyStepper {
  public:
    MyStepper(AccelStepper* stepper, int sleepPin);
    void goTo(int pos);
    String goHome();
    String goAway();
    void setHomeSensor(int pin);
    void setAwaySensor(int pin);
    void setSensor(int pin1, int pin2);
    String calibrateStepper();
    bool isCalibrated();
    void sleep();
    void setSleepPin(int pin);
    void wake();
  private:
    int _sensor1;
    int _sensor2;
    int _homeSensor;
    int _homePosition;
    int _awaySensor;
    int _awayPosition;
    int _sleepPin;
    bool _awaySet = false;
    bool _homeSet = false;
    bool _calibrated = false;

};

#endif
