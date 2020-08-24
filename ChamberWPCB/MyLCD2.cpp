/*
   I made this library to streamline using a 20x4 display for both printing and in conjunction with a keypad

   MyLCD2() requires &keypad and &lcd

   displayInfo() take four strings in and prints one on each row of the LCD screen

   displayTime() takes a DateTime and displays the HH:MM in the bottom right of the LCD

   intialise() must be called to start the LCD

   getInt() takes a string input, and returns an int.  The menu acts dynamically and allows for the user to see what they are inputting, and clear the value
   if they made a mistake (*), or confirm the value (#)


*/




#include <Arduino.h>
#include "MyLCD2.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RTClib.h>



//LiquidCrystal_I2C *_clcd;
//Keypad *_ckeypad;

MyLCD2::MyLCD2(Keypad * ckeypad, LiquidCrystal_I2C * lcd) {
  _ckeypad = ckeypad;
  _clcd = lcd;
}

void MyLCD2::displayInfo(String l1, String l2, String l3, String l4) {
  _clcd->clear();
  _clcd->setCursor(0, 0);
  _clcd->print(l1);
  _clcd->setCursor(0, 1);
  _clcd->print(l2);
  _clcd->setCursor(0, 2);
  _clcd->print(l3);
  _clcd->setCursor(0, 3);
  _clcd->print(l4);

}

void MyLCD2::displayTime(DateTime now) {
  _clcd->setCursor(15, 3);
  _clcd->print(String(twoChar(now.hour()) + ":" + (twoChar(now.minute()))));
}

void MyLCD2::initialise() {
  _clcd->init();
  _clcd->backlight();
  _clcd->setCursor(0, 0);
  _clcd->print("MYLCD Started");
  delay(1000);
  _clcd->clear();
}

String MyLCD2::twoChar(int in) {
  if (in < 10) {
    return "0" + String(in);
  }
  else
    return String(in);
}

int MyLCD2::getInt(String question) {
  char key;
  while (true) {
    _clcd->setCursor(0, 0);
    _clcd->print(question);
    key = getinput();
    if (key == '#') {
      _val = _tempVal.toInt();
      clearVal();
      return _val;
    } else if (key == '*') {
      clearVal();
    } else {
      printInput(key);
    }
  }
}

//Helper class, waits until it registers an inputted key
//returns the value of the key
char MyLCD2::getinput() {
  char tempkey;
  tempkey = _ckeypad->getKey();
  while (true) {
    tempkey = _ckeypad->getKey();
    if (tempkey != NO_KEY) {
      return tempkey;
    }
  }
}

//clears clcd and tempVal Variable
void MyLCD2::clearVal() {
  _tempVal = "";
  _clcd->clear();
}

//Takes inputted char, adds to to saved tempVal and prints on second clcd line
void MyLCD2::printInput(char in) {
  _clcd->setCursor(0, 1);
  _tempVal = _tempVal + in;
  _clcd->print(_tempVal);
}
