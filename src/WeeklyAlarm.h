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

#include "Arduino.h"
#include <Time.h>
#include <ArduinoJson.h>

enum class AlarmType : uint8_t {SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY,SATURDAY, WEEK,  WEEK_END, ALL_DAYS};

#define ON 1
#define OFF 0

class AlarmObj {
public:
  AlarmObj();
  void set(AlarmType _type, bool _almSwitch, int8_t _wHour, int8_t _wMin);
  JsonObject& getJSON(JsonBuffer& jsonBuffer);
  void parseJSON(JsonObject& alarmObj);
  const char* weekTypeToString();
  AlarmType stringToWeekType(const char* weekTypeInput);
  char const *isOnOff();
  bool OnOffToBool(const char* _switch);
  void toggle();
  void printTo(Stream &stream);
protected:

private:
  friend class WeeklyAlarm;
  AlarmType type;
  bool almSwitch;
  int8_t wHour;
  int8_t wMin;
  virtual void callbackHandler()=0;
  AlarmObj *nextAlarm;
  unsigned long target;
  static void prettyPrintClock(int hour, int minute, Stream &stream);
};

///////////////////////////////////////////
class Alarm : public AlarmObj {
public:
  void setCallback(void (*_callback)());
private:
  void callbackHandler();
  void (*callback)();
};

class AlarmInt : public AlarmObj {
public:
  void setCallback(void (*_callback)(int), uint8_t _callbackValue);
private:
  void callbackHandler();
  void (*callback)(int);
  uint8_t callbackValue = 0;
};
//////////////////////////////////////////

class WeeklyAlarm {
public:
  WeeklyAlarm();
  void add(AlarmObj &alarm);
  void remove(AlarmObj &alarm);
  void handler();
  void prettyPrintTime(time_t time, Stream &stream);

private:
  AlarmObj *alarmHead;
  unsigned long _lastAlarmCheck = 0;
  time_t getTimer(AlarmObj &alarm);
  int8_t getDayToGo(uint8_t today, uint8_t target);
  bool todaysTimeIsPast(TimeElements now, AlarmObj &alarm);  
};
#endif
