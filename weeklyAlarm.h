/*
WeeklyAlarm is a library inspired by a programmable thermostat with scalable memory and 4 modes.

first mode is single day use, and is listed 0 to 6, corresponding to sunday to saturday.
mode 7 is weekdays only (monday to friday). 
mode 8 is weekend.
mode 9 is the whole week.

  Created on 26/06/15
   By Nitrof

Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*///////////////////////////////////////////////////////////////

#ifndef WeeklyAlarm_h
#define WeeklyAlarm_h

#if (ARDUINO >= 100)
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <Time.h>
#include <LinkedList.h>
#include <ArduinoJson.h>

#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURSDAY 6
#define WEEK 7
#define WEEK_END 8
#define ALL_DAYS 9

#define ON 1
#define OFF 0

class Alarm {
public:
  Alarm();
  Alarm(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) );

  int8_t type;
  bool almSwitch;
  int8_t wHour;
  int8_t wMin;
  void (*callback)(int);
};


class WeeklyAlarm : public Alarm {
public:
  WeeklyAlarm(uint8_t numAlrm);
  void add();
  void add(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) );
  void set(int8_t id, int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) );
  void set(int8_t id, int8_t type, bool almSwitch, int8_t wHour, int8_t wMin ); //for backup restore
  void set(int8_t id, String strType, String strAlmSwitch, int8_t wHour, int8_t wMin); //this set is for web request input

  void handler();
  void toggle(uint8_t id);


  String isOnOff(uint8_t id);
  String weekType(uint8_t id);
  uint8_t almHour(uint8_t id);
  uint8_t almMin(uint8_t id);

  void printAlarm(uint8_t id);

  JsonObject& backupAlarm(int8_t id, JsonBuffer& jsonBuffer);
  void restoreAlarm(int8_t id, JsonObject& output);
private:
  LinkedList<Alarm> alarm;

  bool nestedBool(bool b0, bool b1, bool b2, bool b3);  
  long _lastAlarmCheck = millis();
  bool compareWeekDay(uint8_t type, uint8_t thisDay);

  int8_t stringToWeekType(String weekTypeInput);
  bool stringToAlmSwitch(String OnOffInput);
};
#endif
