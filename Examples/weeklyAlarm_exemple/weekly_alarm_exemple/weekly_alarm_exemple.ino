#include <TimeLib.h>
#include <WeeklyAlarm.h>

    //create instance of alarm with 3 alarm
WeeklyAlarm alarm(2); 

void setup() {
  Serial.begin(9600);
  setTime(9, 00, 0, 11, 9, 2016);  
      //set alarm: (id, type, almSwitch, Hour, Min, callback) 
      //type:SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURSDAY, WEEK, WEEK_END, ALL_DAYS 
  alarm.set(0, ALL_DAYS, ON, 9, 1,callback1); //callback have to be declare --> function(int), the return param will be the alarm id
  alarm.set(1, SUNDAY, ON, 9, 2,callback2);
      // you can add more alarm,
      //alarm.add(); //blank alarm with default value, it have to be have to use set()
      //or --->
  alarm.add(WEEK_END, ON, 9, 3,callback2); //add a new alarmm with all parameter set. Id will be at the end                                        
  delay(5000);
}

void loop() {
  alarm.handler();//manage time callback of all alarm
  digitalClockDisplay();
  delay(5000);
}

     //weeklyAlarm return the ID of the alarm into the index var
void callback1(int index){
  Serial.println("alarm 0 have been trigged");
  printAlarm(index);
}

void callback2(int index){ 
  if(index == 1) Serial.println("alarm 1 have been trigged");
  if(index == 2) Serial.println("alarm 2 have been trigged");
  printAlarm(index);
}

void printAlarm(int index){ //use weekType, almHour and almMin for troubleshoot and for JSON object for web server
  Serial.print(alarm.weekType(index));
  Serial.print(" ");
  Serial.print(alarm.almHour(index));
  Serial.print(":");
  Serial.println(alarm.almMin(index));
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
