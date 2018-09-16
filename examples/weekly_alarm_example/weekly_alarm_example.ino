#include <TimeLib.h>
#include <weeklyAlarm.h>


    //create instance of alarm with 2 alarms
WeeklyAlarm alarm(2); 

void setup() {
  Serial.begin(9600);

  //setTime( hr, min, sec, day,  month,  yr);
  setTime(9, 00, 0, 8, 9, 2018);  
  digitalClockDisplay();
      //set alarm: (id, type, almSwitch, Hour, Min, callback) 
      //type:SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURSDAY, WEEK, WEEK_END, ALL_DAYS 
  alarm.set(0, ALL_DAYS, ON, 9, 01, callback1); //callback has to be declared as "void functionName(int)", the return param will be the alarm id
  alarm.set(1, SUNDAY, ON, 9, 2, callback2);
      // you can add more alarms,
      //alarm.add(); //blank alarm with default values, it has to be added, before you use set()
      //or you add and set in one step:
  alarm.add(WEEK_END, ON, 9, 3,callback2); //add a new alarm with the parameter set. Id will be the next free value   
   
  alarm.printAlarm(0, Serial);
  alarm.printAlarm(1, Serial);
  alarm.printAlarm(2, Serial);                                    
  delay(5000);
}

void loop() {
  alarm.handler();//manage time callbacks of all alarms
  digitalClockDisplay();
  delay(5000);
}


     //weeklyAlarm passes the ID of the triggered alarm as parameter
void callback1(int index){
  Serial.println("alarm 0 has been triggered");
  //printAlarm(index);
}

void callback2(int index){ 
  digitalClockDisplay();
  if(index == 1) Serial.println("alarm 1 has been triggered");
  if(index == 2) Serial.println("alarm 2 has been triggered");
}


void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
