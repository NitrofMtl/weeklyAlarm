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
#include <ArduinoJson.h>

enum alarmType {SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY,SATURSDAY, WEEK,  WEEK_END, ALL_DAYS};

#define ON 1
#define OFF 0

class Alarm {
public:
  Alarm();
  Alarm(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) );
  Alarm(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int), int8_t wId );
  int8_t type;
  bool almSwitch;
  int8_t wHour;
  int8_t wMin;
  void (*callback)(int);
  int8_t id;
  Alarm *nextAlarm;
  unsigned long target;
};


class WeeklyAlarm {
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

  
  void printAlarm(uint8_t id, Stream &stream);
  

  JsonObject& backupAlarm(int8_t id, JsonBuffer& jsonBuffer);
  void restoreAlarm(int8_t id, JsonObject& output);
private:
  Alarm *alarmHead;
  int idIndex = 0;

  long _lastAlarmCheck = millis();

  time_t WeeklyAlarm::getTimer(Alarm &alarm);

  int8_t stringToWeekType(String weekTypeInput);
  bool stringToAlmSwitch(String OnOffInput);

  int8_t getDayToGo(uint8_t today, uint8_t target);
  bool todaysTimeIsPast(TimeElements now, Alarm &alarm);
  Alarm *getAlarmById(int id);

  void printAlarm(Stream &stream, Alarm *alarm);
};
#endif
