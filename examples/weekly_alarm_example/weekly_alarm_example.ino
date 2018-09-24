#include <TimeLib.h>
#include <WeeklyAlarm.h>


    //create instance
WeeklyAlarm weeklyAlarm; //main instance
//then all alarm needed
Alarm alarm1;
AlarmInt alarm2WithInt;

//or create your own alarm class to fit your callback requirement:
class myAlarmWithString : public AlarmObj {
public:
  //callbackHandler will be call by weeklyAlarm at time.
  // It is a pure virtual function and MUST be define in AlarmObj child
  void callbackHandler() { mycallback("an example of string"); }; 
  void mycallback(char* msg) { Serial.println(msg); }
  //-->void setCallback( /*your callback form*/ ) { /*You have to implement how to setup your callback*/ }
};
//create an object:
myAlarmWithString myAlarm;
//function myAlarm.set(alarmType, switch, hour, minute) belong to mother class and is use like other method.



void setup() {
  Serial.begin(9600);

  //setTime( hr, min, sec, day,  month,  yr);
  setTime(9, 01, 0, 8, 9, 2018);  
  weeklyAlarm.prettyPrintTime(now(), Serial);
  //set callback for each alarm
  alarm1.setCallback(callbackPlain);
  alarm2WithInt.setCallback(callback1, 0);
  //set time for each alarm
    //AlarmType options: SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY,SATURSDAY, WEEK,  WEEK_END, ALL_DAYS
        //  (AlarmType::, switch, hour, minute)
  alarm1.set(AlarmType::ALL_DAYS, ON, 9, 01);
  alarm2WithInt.set(AlarmType::WEEK_END, ON, 9, 3);
  //add all alarm to the handler
  weeklyAlarm.add(alarm1);
  weeklyAlarm.add(alarm2WithInt);
   
  alarm1.printTo(Serial);
  alarm2WithInt.printTo(Serial);
                                    
  delay(5000);
}

void loop() {
  weeklyAlarm.handler();//manage time callbacks of all alarms
  weeklyAlarm.prettyPrintTime(now(), Serial);
  delay(5000);
}


void callbackPlain() {
  Serial.println("alarm without any arguments");
}

     //weeklyAlarm passes the ID of the triggered alarm as parameter
void callback1(int index){
  Serial.println("alarm 0 has been triggered");
}

void callback2(int index){ 
  if(index == 1) Serial.println("alarm 1 has been triggered");
  if(index == 2) Serial.println("alarm 2 has been triggered");
}
