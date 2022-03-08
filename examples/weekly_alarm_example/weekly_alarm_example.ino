#include <Arduino.h>

#include <TimeLib.h>

#include "WeeklyAlarm.h"

/*
Callback with int argument have been removed.
Now to configure the alarm, use WeeklyAlarm obj instead of AlarmObj
void* pointer have been replace by template argument, 
so if you have to need any argument to your callback,
you could use a functor or a lambda function with capture.
  
*/

WeeklyAlarm alarm1;
WeeklyAlarm alarm2;
WeeklyAlarm alarm3;
WeeklyAlarm alarm4;

void callbackPlain() {
  Serial.println("alarm1, callback void*. \n Alarm timer are reset in the handler before calling the callback. \n So you can call prettyPrint again to check when will be the next trigg.");
  Serial.print("Alarm1 set: ");
  alarm1.prettyPrintAlarm(Serial);
}

struct FunctorExample {
  FunctorExample(int x) : x(x) {}
  void operator()() { 
    Serial.print("functor callback have been call with x value: "); 
    Serial.println(x);
    }
  int x = 0;
};

const char* message = "This message have been send by lambda function";


void setup() {
  Serial.begin(9600);
  Serial.println("start");
  //setTime( hr, min, sec, day,  month,  yr);
  setTime(11, 24, 0, 12, 9, 2018);  

  // add( callable ) : set the callback of the alarm could be : function pointer, functor or lambda with or without capture
  // dayEnable( timeDayOfWeek_t... ) : put all days of week (timeDayofWeek_t) the you want to enable
  // set ( hour, minute ) : set the time of day that alarm will be trigged
  // alarmOn() : enable the alarm (days enable have to be set first)
  alarm1.add(callbackPlain).dayEnable( dowWednesday, dowFriday).set(11,25).alarmOn();

  
  // alarmOff() : Disable the alarm, other parameter kept untouch
  alarm1.alarmOff();
  // alarmToggle() : Toggle On/Off the alarm
  alarm1.alarmToggle();

  // dayDisable( timeDayOfWeek_t... ) : put all days of week (timeDayofWeek_t) the you want to disable
  alarm1.dayDisable(dowWednesday);
  // dayToggle( timeDayOfWeek_t... ) : put all days of week (timeDayofWeek_t) the you want to toggle on/off
  alarm1.dayToggle(dowWednesday);

  //isEnable() : check if alarm is enable
  if (alarm1.isEnable()) {
    Serial.println("alarm1 is enable!");
  }

  //isDayEnable( timeDayOfWeek_t ) : check if a specific day is active on an the alarm
  if ( alarm1.isDayEnable(dowWednesday) ) {
    Serial.println("alarm1 wednesday is enable!");
  }



  FunctorExample myFunctor(24);
  alarm2.add(myFunctor).dayEnable( dowWednesday, dowThursday ).set(11, 26).alarmOn();

  //lambda example
  alarm3.add( [&](){Serial.println(message);} ).dayEnable( dowSunday, dowMonday, dowTuesday, dowWednesday ).set(12, 50).alarmOn();
  
  //example of remove
  alarm4.add(callbackPlain);
  // remove() : Remove the alarm, all parameters and callback will be deleted
  alarm4.remove();

  
  Serial.print("Time set:   ");
  //print nice format of time_t object
  WeeklyAlarm::prettyPrintTime(now(), Serial);
  Serial.print("Alarm1 set: ");
  // prettyPrintAlarm( Stream ) : print nice format of alarm setup
  alarm1.prettyPrintAlarm(Serial);
  Serial.print("Alarm2 set: ");
  alarm2.prettyPrintAlarm(Serial);
  Serial.print("Alarm3 set: ");
  alarm3.prettyPrintAlarm(Serial);

  // toJSON() : Build a JSON string of the alarm settings
  Serial.println(alarm1.toJSON());

  //getDaysEnable() : Get the Days Enable object flag bit 1 to 7 = dowSunday to dowSaturday, bit 0 is for alarm enable flag
  uint8_t alarm1DaysEnableFlag = alarm1.getDaysEnable();
  
  delay(5000);
}

unsigned long timer = 0;

void loop() {
  if ( timer + 60000 < millis() ) { //methode to call the handler each minute
  Serial.println("WeeklyAlarm Handler");
    //handler() : Main handler, call the function every minute to monitor when a callback is to be trigged
    WeeklyAlarm::handler();
    timer = millis();
  }
  

}

 