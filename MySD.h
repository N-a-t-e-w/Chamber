#ifndef MySD_h
#define MySD_h
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"

class MySD {
  public:
    MySD(RTC_DS3231* rtc, int cs);
    bool initialise();
    String logData(String action, String data);
    String deleteFile(String filename);
    void setFileName(String filename);
  private:
    RTC_DS3231 *_rtc;
    String getTime();
    int _cs;
    String _filename;
};

#endif
