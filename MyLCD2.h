#ifndef MyLCD2_H
#define MyLCD_H
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Arduino.h>
#include <RTClib.h>


class MyLCD2 {
  public:
    MyLCD2(Keypad * ckeypad, LiquidCrystal_I2C * lcd);
    int getInt(String question);
    void initialise();
    char getinput();
    void displayInfo(String l1, String l2, String l3, String l4);
    void displayTime(DateTime now);
  private:
    String twoChar(int in);
    LiquidCrystal_I2C *_clcd;
    Keypad *_ckeypad;
    void clearVal();
    void printInput(char in);
    String _tempVal;   //temporary value storing
    int _val;
    char _keys[4][3] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};

};


#endif
