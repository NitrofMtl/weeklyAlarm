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




#include <weeklyAlarm.h>
#include <TimeLib.h>

weeklyAlarm testAlarm;

void setup() {
  Serial.begin(9600);
  setTime(10, 10, 0, 14, 6, 15); //set time hour, min, sec, day, month, year
  testAlarm.weeklyAlarmInit();
  testAlarm.setWeeklyAlarm(0, 1, 1, 10, 15); //alarm tag, type alarm, alarm switch, hour, minute
  testAlarm.setWeeklyAlarm(2, 8, 1, 4, 35);
  testAlarm.setWeeklyAlarm(6, 10, 1, 20, 00);

}

time_t prevDisplay = 0; // when the digital clock was displayed


void digitalClockDisplay() {
  // digital clock display of the time
  if (Serial.available() > 0) ; {
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print("/");
    Serial.print(month());
    Serial.print("/");
    Serial.print(year());
    Serial.println();
  }
}

void printDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

////////////////////////////////////////////////////////////////////

void loop() {
  digitalClockDisplay();
  testAlarm.weeklyAlarmMon();
  for ( int i = 0; i <= 9; i++ ) {
    alarmTrigger(i);
  }
  exemepleSwitchAlarm();
  delay(1000);
}

void alarmTrigger(int numAlarm) { // excute if trigger as beren turn on by AlarmMon
  if ( testAlarm.trigger(numAlarm) == 1 ) {
    // function to do goes here
    if (Serial.available() > 0) {
      Serial.println("alarm trigerred # ");
    }
    testAlarm.printAlarm(numAlarm);  // print alarm time and type
  }
}

void exemepleSwitchAlarm() {// this section is a exemple how to use the fonction weeklyAlarmSwitch,
  // on the serial port, if you type "0", it turn off the selected alarm, "1" turn on and "2" toggle it.
  if (Serial.available() > 0) {
    String inbyte = Serial.readStringUntil('\n');
    if (inbyte == ("0")) {//input form serial port
      testAlarm.weeklyAlarmSwitch(0, 0); //  weeklyAlarmSwitch(#alarm, option ( 0=off, 1=on)
    }
    else if (inbyte == ("1")) { //input form serial port
      testAlarm.weeklyAlarmSwitch(0, 1);
    }
    else if (inbyte == ("2")) { //input form serial port
      testAlarm.weeklyAlarmSwitch(0); // param = #alarm, toggle selected alarm on/off
    }
  }
}
