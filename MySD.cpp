/*
   This library is used to help with logging data on to an SD card

   MySD() requires &rtc and CS pin

   initialise() starts sd, returns false if error

   setFileName() sets _filename for use when writing to a file

   deleteFile() is used to delete a file by name
*/

#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"
#include "MySD.h"


//Intitial setup, gets rtc and cs
MySD::MySD(RTC_DS3231 *rtc, int cs) {
  pinMode(cs, OUTPUT);
  _cs = cs;
  _rtc = rtc;
}

//Starts sd, returns false if error
bool MySD::initialise() {
  return SD.begin(_cs);

}

//sets filename for writing to file
void MySD::setFileName(String filename) {
  _filename = filename;
}

//Deletes file with name, returns string with info
String MySD::deleteFile(String fname) {
  if (SD.exists(fname)) {
    if (SD.remove(fname) == true) {
      return "File succesfully removed";
    } else {
      return "File not removed";
    }
  } else {
    return "File doesnt exist";
  }
}

//Takes 2 strings, could be changed to 1 but leaving two for now.
String MySD::logData(String action, String data) {
  File file = SD.open(_filename, FILE_WRITE);
  DateTime now = _rtc->now();
  String thisTime = String(now.year() + String("/") + now.month() + String("/") + now.day() + String(",") + now.hour()) + String(":") + String(now.minute()) + String(":") + String(now.second());
  file.println((thisTime + String(",") + action + String(",") + data + String(",")));
  file.close();
  return String(thisTime + String(",") + action + String(",") + data + String(","));

}
