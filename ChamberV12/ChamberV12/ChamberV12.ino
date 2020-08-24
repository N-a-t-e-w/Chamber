/*
  MODULES

  20x4 DISPLAY  ((USED WITH MY lCD LIBRARY))
  Connected via I2C

  KEYPAD        ((USED WITH MY LCD LIBRARY))
  PINS LEFT -> RIGHT (WIRED AS 4x3)
  44 42 40 38 36 34 32

  RTC
  Connected via I2C

  SD READER ((USING MY SD LIBRARY))
  Connected via MOSI MISO SCK
  CS Connected to PIN 3

  HALL EFFECT Sensor
  RED - VCC
  BLUE - GROUND
  ORANGE(home/closed) - 10
  YELLOW(away/open) - 12

  STEPPER Driver a4988 ((USING MY STEPPER LIBRARY))
  1A - Green
  1B - Brown
  2A - Red
  2B - Blue
  DIR PIN 27
  STEP PIN 26
  SLEEP PIN 4
  RES pulled HIGH


  DHT11
  1 VCC
  2 PU VCC PIN 2
  3 -
  4 GND


*/
#include <Keypad.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include "MyStepper.h"
#include "MyLCD2.h"
#include "MySD.h"


//Time for flush
#define flushTime 10

//Pins for keypad
#define kpadr1 31
#define kpadr2 33
#define kpadr3 35
#define kpadr4 37
#define kpadc1 39
#define kpadc2 41
#define kpadc3 43

//Hall effect sensors
#define awaySensor 3
#define homeSensor 2

//Stepper pins
#define dirPin 10
#define stepPin 9
#define sleepPin 8

//SD PIN
#define cs 49


//Create Stepper
AccelStepper steppermotor(1, stepPin, dirPin);

//create CLD
LiquidCrystal_I2C clcd(0x27, 20, 4);

//Keypad variables
byte rowPins[4] = {kpadr1, kpadr2, kpadr3, kpadr4}; // connect to the row pinouts of the keypad
byte colPins[3] = {kpadc1, kpadc2, kpadc3};   // connect to the column pinouts of the keypad
char keys[4][3] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};

//Create Keypad
Keypad ckeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

//Create MyStepper
MyStepper mystepper(&steppermotor, sleepPin);

//Create MyLCD
MyLCD2 myLcd(&ckeypad, &clcd);

//create RTC
RTC_DS3231 rtc;

//Create MySD
MySD mysd(&rtc, cs);

//FileName
String fname = "";

//Setup Variables
int programNum;  //set as the program number
bool boolProgramSelect = false; //false until known program is chosen, then set to true;
bool boolCheckTime = false; //false until time is confirmed

//Variables for setting RTC
int yr;
int mth;
int dy;
int hr;
int mn;


//StandardRun() variables and flags
bool boolVariables = false;
bool boolFan = false;
int numSamples;
int timeBetweenSamples;
int initDelay;
int sampleLength;


void setup() {
  Serial.begin(9600);
  clcd.setCursor(0, 0);
  clcd.print("WELCOME");
  delay(2000);
  myLcd.initialise();  //starts myLcd
  mystepper.setHomeSensor(homeSensor); //sets homeSensor for stepper
  mystepper.setAwaySensor(awaySensor);  //sets awaySensor for stepper
  CheckRTCandSD(); //makes sure RTC and SD card are working
  checkTime(); //Confirms time
  menu();

}

void loop() {
  switch (programNum) {   //runs slected program
    case 1:
      StandardRun();
      break;
    case 2:
      TestRun();
      break;
    default:
      break;
  }
}

//For testing without running stepper motor
void TestRun() {
  while (!boolVariables) {
    numSamples = myLcd.getInt("How many samples?");
    sampleLength = myLcd.getInt("Duration (min)");
    timeBetweenSamples = myLcd.getInt("Time Between? (min)");
    initDelay = myLcd.getInt("InitialDelay? (min)");
    boolVariables = true;
  }
  for (int i = 0; i < initDelay; i++) {
    myLcd.displayInfo(String("Waiting: " + String(initDelay - i) + " mins"), "until First sample", "", "");
    myLcd.displayTime(rtc.now());
    delay(minuteToMili(1));
  }
  delay(minuteToMili(initDelay));//Initial Delay
  for (int i = 0; i < numSamples; i++) {
    //flushChamber();
    for (int x = 0; x < sampleLength; x++) {
      mysd.logData("Sample logging", logSensors());
      displayCurrentInfo(x, i);
      delay(minuteToMili(1));
    }
    for ( int x = 0; x < timeBetweenSamples; x++) {
      mysd.logData("Idle logging", logSensors());
      delay(minuteToMili(1));
    }
  }
  displayCurrentInfo(numSamples - 1, sampleLength);
  myLcd.displayInfo("done", "", "", "");
  while (true);
}


void StandardRun() {
  while (!boolVariables) {
    numSamples = myLcd.getInt("How many samples?");
    sampleLength = myLcd.getInt("Duration (min)");
    timeBetweenSamples = myLcd.getInt("Time Between? (min)");
    initDelay = myLcd.getInt("InitialDelay? (min)");
    boolVariables = true;
  }
  for (int i = 0; i < initDelay; i++) { //Initial delay, prints lcd to display what is going on
    myLcd.displayInfo(String("Waiting: " + String(initDelay - i) + " mins"), "until First sample","", "");
    myLcd.displayTime(rtc.now());
    delay(minuteToMili(1));
  }
  for (int i = 0; i < numSamples; i++) {//run for each sample
    flushChamber();
    for (int x = 0; x < sampleLength; x++) {
      mysd.logData("Sample logging", logSensors());
      displayCurrentInfo(x, i);
      delay(minuteToMili(1));
    }
    for ( int x = 0; x < timeBetweenSamples; x++) {//After the "sample" while idling, data is still logged but the fan does not run (saves power)
      myLcd.displayInfo(String("SAMPLE " + String(i + 1) + " OF " + String(numSamples) + " mins"),
                        String("ELAPSED " + String(sampleLength) + " OF " + String(sampleLength)), "Currently Idle", "");
      myLcd.displayTime(rtc.now());
      mysd.logData("Idle logging", logSensors());
      delay(minuteToMili(1));
    }
  }
  displayCurrentInfo(numSamples - 1, sampleLength);
  while (true);

}

//Helper function to display info while running
void displayCurrentInfo(int x, int i) {
  myLcd.displayInfo(String("SAMPLE " + String(i + 1) + " OF " + String(numSamples)),
                    String("ELAPSED " + String(x) + " OF " + String(sampleLength) + " mins"),
                    "",
                    String(""));
  myLcd.displayTime(rtc.now());
}

//Awake and open lid
void openLid() {
  mystepper.wake();
  mystepper.goAway();
  mysd.logData("Opened lid", logSensors());
}

//Close lid and sleep
void closeLid() {
  mystepper.goHome();
  mystepper.sleep();
  mysd.logData("Closed lid", logSensors());
}

//Opens the chamber, runs the fan for an amount of time, and closes the chamber
void flushChamber() {
  myLcd.displayInfo("Flushing Chamber", "Opening Lid", "", "");
  openLid();
  delay(secToMili(flushTime)); 
  myLcd.displayInfo("Flushing Chamber", "Closing Lid", "", "");
  closeLid();
  mysd.logData("Chamber Flushed", logSensors()); //Logging Chamber flush
}

//convert int minutes into equal long ms
long minuteToMili(int mins) {
  return mins * 60000;
}

//converts int seconds to equal long ms
long secToMili(int sec) {
  return sec * 1000;
}

//add sensors here to be logged on call
String logSensors() {
  String data = "";
  data = String("");
  return data;
}

//For choosing the program, sets programNum to chosen value 
void menu() {
  while (!boolProgramSelect) {
    switch (myLcd.getInt("Choose Program")) {
      case 1: //standardRun()
        if (mystepper.isCalibrated()) {
          boolProgramSelect = true;
          programNum = 1;
          clcd.clear();
          clcd.print("1 Selected");
          delay(1000);
          break;
        } else {
          myLcd.displayInfo("Calibrate Stepper", "Option 3", "", "");
          delay(4000);
          clcd.clear();
          break;
        }
      case 3: //Calibrate stepper
        myLcd.displayInfo("Make sure chamber", "is clear of objects", "calibrating...", "reset to cancel");
        delay(5000);
        mystepper.calibrateStepper();//Calibrates stepper
        mysd.logData("Calibrated Motor", logSensors()); //logs calibration
        myLcd.displayInfo("CALIBRATED", "", "", "");
        delay(2000);
        myLcd.displayInfo("", "", "", "");
        break;
      default:
        clcd.clear();
        clcd.print("Unknown Selection");
        delay(2000);
        break;
    }
  }
}


//Checks to see if current time is correct, if not, calls setTime to allow user to set the time
void checkTime() {
  while (!boolCheckTime) {
    DateTime now = rtc.now();
    confirmTimeScreen(now);
    switch (myLcd.getinput()) {
      case '*':
        setTime();
        break;
      case '#':
        boolCheckTime = true;
        fname = twoChar(now.month()) + twoChar(now.day()) + twoChar(now.hour()) + twoChar(now.minute()) + ".csv";
        mysd.setFileName(fname);
        myLcd.displayInfo("FILE CREATED",fname,"","");
        delay(2000);
        clcd.clear();
        break;
      default:
        break;
    }
  }
}

//prints a screen to confirm time to LCD
void confirmTimeScreen(DateTime now) {
  myLcd.displayInfo(
    String("Correct date / time?"),
    String(String(now.year()) + "/" + twoChar(now.month()) + "/" + twoChar(now.day())),
    String(twoChar(now.hour()) + ":" + twoChar(now.minute())),
    String("*-NO           #-YES"));
}

//Gets date and time info from user
void setTime() {
  clcd.clear();
  yr = myLcd.getInt("YEAR?");
  mth = myLcd.getInt("MONTH?");
  dy = myLcd.getInt("DAY?");
  hr = myLcd.getInt("HOUR? (24 HOUR)");
  mn = myLcd.getInt("MINUTE?");
  rtc.adjust(DateTime(yr, mth, dy, hr, mn, 0));
  delay(100);
}

//cheacks RTC and SD are working
void CheckRTCandSD() {
  Serial.print("RTC AND SD");
  if (!rtc.begin()) { //Checks to see if RTC is working, if not, freeze the program
    myLcd.displayInfo("ERROR","CHECK RTC","","");
        Serial.print("RTC");

    while (true) {
    }
   

  }   
Serial.print("A");
  if (!mysd.initialise()) {//Checks to see if SD is working, if not, freeze the program
    myLcd.displayInfo("ERROR","CHECK SD CARD","","");
    Serial.print("SD");
    while (true) {

  }    }    

}

//never deals with negative
//Formats int to string from "5" -> "05" or "15" -> "15"
String twoChar(int in) {
  if (in < 10) {
    return "0" + String(in);
  }
  else
    return String(in);
}
