/*
WeeklyAlarm is a library inpire like programable thermostat whit 10 memory and 4 mode.

first mode is sigle day use, and is listed 1 to 7, coresponding to sunday to saturday.
mode 8 is for week only, so monday to friday. 
mode 9 is for week end.
mode 10 is for all day.

The second input of the setup function is a switch so you can turn of the alarm without lossing it.

Function avalaible to use the library is:
  - weeklyAlarmInit(); //initialize the alarm matrix
  - setWeeklyAlarm(alarm tag, Switch, Mode, hour, minute); 
  - weeklyAlarmMon(); // real time monitoring if an alarm have to be trigger. check it every 5 minute
  - alarmTrigger(alarm tag); // if mon have set to true an alarm to be trigger, this will run the function associate with the alarm
  - weeklyAlarmSwitch(alarm tag); toggle on/off an alarm
  - weeklyAlarmSwitch(alarm tag, 1); toggle on an alarm
  - weeklyAlarmSwitch(alarm tag, 0); toggle off an alarm

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

   

#ifndef weeklyAlarm_h
#define weeklyAlarm_h



#include "Arduino.h"
#include "Time.h"

class weeklyAlarm {
public:
void weeklyAlarmInit();
void setWeeklyAlarm(int8_t alarmTag, int8_t alarmType, int8_t alarmSwitch, int8_t wHour, int8_t wMin);
void weeklyAlarmMon();
void scanWeeklyAlarm();
void printAlarm(int8_t numAlarm);
void weeklyAlarmSwitch(int8_t numAlarm);
void weeklyAlarmSwitch(int8_t numAlarm, int8_t alarmSwitch);

int8_t trigger(int8_t numAlarm);
int8_t getParam(int8_t alarmTag, int8_t param);

private:
//int weeklyAlarmID;
//const int WAhour = 2;
//const int WAminute = 3;
//const int WAtrigger = 4;
//int weeklyAlarmMatrix[10][5]; //matrix 1 = alarm number, 2 = alarm parameter;

//int _lastAlarmCheck = millis();
String printDigits(int8_t digits);
};


#endif