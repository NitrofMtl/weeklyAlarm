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



#include "Arduino.h"
#include "Time.h"
#include "weeklyAlarm.h"


uint8_t weeklyAlarmID;          //matrix var declare
const int8_t WASwitch = 0;
const int8_t WAType = 1;
const int8_t WAhour = 2;
const int8_t WAminute = 3;
const int8_t WAtrigger = 4;
int8_t weeklyAlarmMatrix[10][5]; //matrix 1 = alarm number, 2 alarm = parameter;

int _lastAlarmCheck = millis();

void weeklyAlarm::weeklyAlarmInit()  {      // var and matrix initializer 
  for ( int8_t i=0 ; i<=9 ; i++ )  {
    for ( int8_t j=0; j<5; j++ )  {
     weeklyAlarmMatrix[i][j]  = 0;
   }
 }
}

void weeklyAlarm::setWeeklyAlarm(int8_t alarmTag, int8_t alarmType, int8_t alarmSwitch, int8_t wHour, int8_t wMin)  {    // program an alarm
  weeklyAlarmMatrix[alarmTag][WASwitch] = alarmSwitch;
  weeklyAlarmMatrix[alarmTag][WAType] = alarmType;
  weeklyAlarmMatrix[alarmTag][WAhour] = wHour;
  weeklyAlarmMatrix[alarmTag][WAminute] = wMin;
  weeklyAlarm::printAlarm(alarmTag);
}

String wDay(int8_t type) {
  String weekType[11]{"null", "Sunday ", "monday ", "tuesday ","wednesday ", "thursday ", "Friday ", "saturday ", "Week ", "Week end ", "All day " };
  return weekType[type];
}

void weeklyAlarm::printAlarm(int8_t numAlarm) { // print alarm input
  if (Serial.available() > 0) {
    String output = "Alarm #";
    if (weeklyAlarmMatrix[numAlarm][WASwitch] == 1){
      int8_t h = numAlarm;
      output += String(h) + " on; ";
    }
    else {
      output += String(numAlarm) + " off; ";
    }
    output += "Alarm type: " + wDay(weeklyAlarmMatrix[numAlarm][WAType]) + "at: ";
    output += String(weeklyAlarmMatrix[numAlarm][WAhour]) + ":" ;
    output += weeklyAlarm::printDigits(weeklyAlarmMatrix[numAlarm][WAminute]);
    Serial.println(output);
  }
}

String weeklyAlarm::printDigits(int8_t digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  String digitOut = "0";
  if (digits < 10){ 
    digitOut += String(digits);
  }  
  else {
    digitOut = String(digits);
  }
  return digitOut;
}

void weeklyAlarm::weeklyAlarmMon(){   // realtime monitoring if any alarm have to be triggered
 if  ( millis() >= (_lastAlarmCheck + 300000) ) {   // check every 5 minute
  //if (Serial.available() > 0) {
  //Serial.println("alarm check"); //for troubleshoot
  //}
  scanWeeklyAlarm(); 
  _lastAlarmCheck = millis();
  }
}


void weeklyAlarm::scanWeeklyAlarm() {   //matrix scan for alarm

  bool wWeekDay, wHour, wMinute = false;

  for (int8_t i = 0; i <= 9; i++) {


    /////////// Scan if weekday is a day alarm
    if (( 1 <= weeklyAlarmMatrix[i][WAType] <= 7) && ( weeklyAlarmMatrix[i][WAType] = weekday() )) { //single day alarm check
      wWeekDay = true;
    }
    else if ( (weeklyAlarmMatrix[i][WAType] == 8) && (( 2 <= weekday()) && (weekday() < -6 )) ) { //monday to friday week alarm check
      wWeekDay = true;
    }
    else if ( (weeklyAlarmMatrix[i][WAType] == 9) && ( weekday() == (1 || 7) )  ) { //weekend alarm check
      wWeekDay = true;
    }
    else if (   weeklyAlarmMatrix[i][WAType] == 10) { //all day alarm check
      wWeekDay = true;
    }
    else {
      wWeekDay = false;
    }

    ///////scan hour and minute to trigger alarm
    if (hour() == weeklyAlarmMatrix[i][WAhour]) { // compare alarm hour
      wHour = true;
    }
    else {
      wHour = false;
    }
    if ( (weeklyAlarmMatrix[i][WAminute] <= minute()) && (minute() <=  (weeklyAlarmMatrix[i][WAminute] + 4 ))) { // scan alarm minute with a gap of 4 minunte
      wMinute = true;
    }
    else {
      wMinute = false;
    }

    if ( (weeklyAlarmMatrix[i][WASwitch] == 1) && wWeekDay && wHour && wMinute ) { ///// trigger alarm number if all condition true
     weeklyAlarmMatrix[i][WAtrigger] = 1;
   }

   else {
     weeklyAlarmMatrix[i][WAtrigger] = 0;
   }
 }
}

int8_t weeklyAlarm::trigger(int8_t numalarm) {    // output of library to sketck
  if ((weeklyAlarmMatrix[numalarm][WASwitch] == 1) && (weeklyAlarmMatrix[numalarm][WAtrigger] == 1)){
    if (Serial.available() > 0) {
      Serial.println("alarm trigerred # ");
    }
    weeklyAlarm::printAlarm(numalarm);  // print alarm time and type
    weeklyAlarmMatrix[numalarm][WAtrigger] = 0;
    return 1;
  }
  else {
   return 0; 
 }
}

void weeklyAlarm::weeklyAlarmSwitch(int8_t numAlarm){ // toggle the alarm on and off
  if (weeklyAlarmMatrix[numAlarm][WASwitch] == 1) {
    (weeklyAlarmMatrix[numAlarm][WASwitch] = 0);
  }
  else {
    weeklyAlarmMatrix[numAlarm][WASwitch] = 1;
  }
 weeklyAlarm::printAlarm(numAlarm); // for debug
}

void weeklyAlarm::weeklyAlarmSwitch(int8_t numAlarm, int8_t alarmSwitch){ // set alarm on or off at will
  weeklyAlarmMatrix[numAlarm][WASwitch] = alarmSwitch;
  weeklyAlarm::printAlarm(numAlarm); // for debug
}

int8_t weeklyAlarm::getParam(int8_t alarmTag, int8_t param){
return weeklyAlarmMatrix[alarmTag][param];
}