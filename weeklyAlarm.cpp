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


#if (ARDUINO >= 100)
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <weeklyAlarm.h>

#include <Time.h>
#include <LinkedList.h>
#include <ArduinoJson.h>

int _lastAlarmCheck = millis();

///////////////////////////////////////
/**/          /*  alarm data class  */
Alarm::Alarm(){
  type = ALL_DAYS;
  almSwitch = 0;
  wHour = 0;
  wMin = 0;
}

Alarm::Alarm(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) ) :
  type(type),
  almSwitch(almSwitch),
  wHour(wHour),
  wMin(wMin),
  callback(_callback)
{
}

/**/
///////////////////////////////////////
      /*  weekly Alarm main class   */

WeeklyAlarm::WeeklyAlarm(uint8_t numAlrm) : alarm(numAlrm,Alarm()){
}

void WeeklyAlarm::add(){
  alarm.add(Alarm()); //push default value constructor in a new alarm
}

void WeeklyAlarm::add(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) ){
  alarm.add(Alarm());
  int8_t addedAlarm = alarm.size()-1;//the alarm added is at the end of vector
  set(addedAlarm, type, almSwitch, wHour, wMin, (_callback));
}

void WeeklyAlarm::set(int8_t id, int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) ){
  alarm[id].type = type;
  alarm[id].almSwitch = almSwitch;
  alarm[id].wHour = wHour;
  alarm[id].wMin = wMin;
  alarm[id].callback =  _callback;
}

void WeeklyAlarm::set(int8_t id, int8_t type, bool almSwitch, int8_t wHour, int8_t wMin ){
  alarm[id].type = type;
  alarm[id].almSwitch = almSwitch;
  alarm[id].wHour = wHour;
  alarm[id].wMin = wMin; 
}

  //this set is use for web server parse
void WeeklyAlarm::set(int8_t id, String strType, String strAlmSwitch, int8_t wHour, int8_t wMin){
  alarm[id].type = stringToWeekType(strType);
  alarm[id].almSwitch = stringToAlmSwitch(strAlmSwitch);
  alarm[id].wHour = wHour;
  alarm[id].wMin = wMin;
}


void WeeklyAlarm::handler(){// realtime monitoring if any alarm have to be triggered
  if  ( !(millis() >= (_lastAlarmCheck + 60000)) ){
    return;  // check every 1 minute
  }
  //Serial.println("alarm check"); //for troubleshoot
  for (int i = 0; i < alarm.size(); i++){
    if(nestedBool( //break nested if at the first false condition
        alarm[i].almSwitch, //check if alarm on
        compareWeekDay(alarm[i].type, weekday()-1), //check if it is the good day, time lib is +1 day...
        alarm[i].wHour == hour(), //check hour
        alarm[i].wMin == minute() //check minute
      )) {
      int index = i;      
      alarm[i].callback(index);
      //Serial.println("alarm trigged, internal message"); ///for troubleshoot
    }
  }  
  _lastAlarmCheck = millis();
}

bool WeeklyAlarm::nestedBool(bool b0, bool b1, bool b2, bool b3){ //break nested if at the first false condition
  if(!b0) return 0;
  if(!b1) return 0;
  if(!b2) return 0;
  if(!b3) return 0;
  else return 1;
}

bool WeeklyAlarm::compareWeekDay(uint8_t type, uint8_t thisDay){
  switch (type){
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    {
      if(type==thisDay) return true;
      break;
    }
    case 7:
    {
      if( (thisDay>=MONDAY) && (thisDay<=FRIDAY) ) return true;
      break;
    }
    case 8:
    {
      if( (thisDay==SATURSDAY) || (thisDay==SUNDAY)) return true;
      break;
    }
    case 9:
    {
      return true;
      break;
    }
    default:
    return false;
  }
}

void WeeklyAlarm::toggle(uint8_t id){
  alarm[id].almSwitch = !alarm[id].almSwitch;
}

String WeeklyAlarm::isOnOff(uint8_t id){
  if(!alarm[id].almSwitch) return "OFF";
  else return "ON";
}

String WeeklyAlarm::weekType(uint8_t id){
  const String weekType[10] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Week", "Week end", "All days" };
  uint8_t wType = alarm[id].type;
  return weekType[wType];
  return "Sunday";
}

uint8_t WeeklyAlarm::almHour(uint8_t id){
  return alarm[id].wHour;
}

uint8_t WeeklyAlarm::almMin(uint8_t id){
  return alarm[id].wMin;
}

void WeeklyAlarm::printAlarm(uint8_t id){ //use weekType, almHour and almMin for troubleshoot and for JSON object for web server
  Serial.print(alarm[id].type);
  Serial.print(" ");
  Serial.print(alarm[id].wHour);
  Serial.print(":");
  Serial.println(alarm[id].wMin);
}

int8_t WeeklyAlarm::stringToWeekType(String weekTypeInput){
  String weekType[10] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Week", "Week end", "All days" };
  int type= 0;
  for(int8_t i = 0; i < 10; i++){
    if(weekTypeInput==weekType[i]) {
      type = i;
    }
  }
  return type;
}

bool WeeklyAlarm::stringToAlmSwitch(String OnOffInput){
  if(OnOffInput=="ON") return ON;
  else return OFF;
}

JsonObject& WeeklyAlarm::backupAlarm(int8_t id, JsonBuffer& jsonBuffer){
    JsonObject& alarmBck = jsonBuffer.createObject();
    alarmBck["switch"] = alarm[id].almSwitch;
    alarmBck["type"] = alarm[id].type;
    alarmBck["hour"] = alarm[id]. wHour;
    alarmBck["minute"] = alarm[id].wMin;
    alarmBck["callback"] = alarm[id].callback;
   return alarmBck;
  }

void WeeklyAlarm::restoreAlarm(int8_t id, JsonObject& alarmBck){
    alarm[id].almSwitch = alarmBck["switch"];
    alarm[id].type = alarmBck["type"];
    alarm[id].wHour = alarmBck["hour"];
    alarm[id].wMin = alarmBck["minute"];
}
