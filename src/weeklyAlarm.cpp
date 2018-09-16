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

#include <weeklyAlarm.h>

int _lastAlarmCheck = millis();

///////////////////////////////////////
/**/          /*  alarm data class  */
Alarm::Alarm(){
  type = ALL_DAYS;
  almSwitch = 0;
  wHour = 0;
  wMin = 0;
  nextAlarm = NULL;
}

Alarm::Alarm(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) ) :
  type(type),
  almSwitch(almSwitch),
  wHour(wHour),
  wMin(wMin),
  callback(_callback),
  nextAlarm(NULL)
{
}

Alarm::Alarm(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int), int8_t wId ) :
  type(type),
  almSwitch(almSwitch),
  wHour(wHour),
  wMin(wMin),
  callback(_callback),
  id(wId)
{
}

/**/
///////////////////////////////////////
      /*  weekly Alarm main class   */

WeeklyAlarm::WeeklyAlarm(uint8_t numAlrm) /*: alarm(numAlrm,Alarm())/*, alarmHead(&alarm[0])*/ {
  alarmHead = new Alarm();
  alarmHead->id = idIndex;
  idIndex++;
  Alarm *temp = alarmHead;
  while ( idIndex < numAlrm ) {
    temp->nextAlarm = new Alarm();
    temp = temp->nextAlarm;
    temp->id = idIndex;
    idIndex++;
  }
}

void WeeklyAlarm::add(){
  Alarm * alarm = alarmHead;
  if (!alarmHead) {
    alarmHead = new Alarm();
    return;
  }
  
  while (alarm->nextAlarm) alarm = alarm->nextAlarm;
  alarm->nextAlarm = new Alarm();
  
  alarm->nextAlarm->id = idIndex;
  idIndex++;
}

void WeeklyAlarm::add(int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) ){
  add();
  set(idIndex-1, type, almSwitch, wHour, wMin, (_callback));
}

void WeeklyAlarm::set(int8_t id, int8_t type, bool almSwitch, int8_t wHour, int8_t wMin ){
  Alarm *alarm = getAlarmById(id);
  alarm->type = type;
  alarm->almSwitch = almSwitch;
  alarm->wHour = wHour;
  alarm->wMin = wMin;
  alarm->id = id;
  alarm->target = getTimer(*alarm); 
  //printAlarm(Serial, alarm);
}

void WeeklyAlarm::set(int8_t id, int8_t type, bool almSwitch, int8_t wHour, int8_t wMin, void (*_callback)(int) ){
  set(id, type, almSwitch, wHour, wMin);
  Alarm *alarm = getAlarmById(id);
  alarm->callback =  _callback;
}

  //this set is use for web server parse
void WeeklyAlarm::set(int8_t id, String strType, String strAlmSwitch, int8_t wHour, int8_t wMin){
  set(id, stringToWeekType(strType), stringToAlmSwitch(strAlmSwitch), wHour, wMin);
}

Alarm *WeeklyAlarm::getAlarmById(int id) {
  Alarm *temp = alarmHead;
  for ( int i = 0; i < id; i++ ) {
    temp = temp->nextAlarm;
  }
  if (!temp) return NULL;
  return temp;
}

time_t WeeklyAlarm::getTimer(Alarm &alarm) {
  TimeElements now;
  time_t timer = ::now();
  breakTime(timer,now);

  //weeklyAlarm enum weekdays start with 0 and time with 1... have to offset it each time use both together
  int dayToGo = 0;
  //get how many day to go until the next alarm
  int8_t type = alarm.type;
  switch(type) { 
    case SUNDAY:
    case MONDAY:
    case TUESDAY:
    case WEDNESDAY:
    case THURSDAY:
    case FRIDAY:
    case SATURSDAY:
    if (type != now.Wday-1) {
      dayToGo = getDayToGo(now.Wday-1, type);
      break;
    }
    if ( todaysTimeIsPast(now, alarm) ) {
      dayToGo = 7;
      break;
    }
    break;

    case WEEK: 
    {
      int targetDay = 0;
      if ( now.Wday-1 < MONDAY || now.Wday-1 > FRIDAY ) {
        targetDay = MONDAY;
        dayToGo = getDayToGo(now.Wday-1, targetDay);
        break;
      }
      if ( todaysTimeIsPast(now, alarm) ) {
        targetDay = now.Wday; //mean +1 day
      } else {
        targetDay = now.Wday-1;
      } 
      if ( targetDay > FRIDAY ) targetDay = MONDAY;
      dayToGo = getDayToGo(now.Wday-1, targetDay);
      break;
    }

    case WEEK_END: 
    {
      int targetDay = 0;
      if ( now.Wday-1 <= MONDAY && now.Wday-1 >= FRIDAY ) {
        targetDay = SATURSDAY;
        dayToGo = getDayToGo(now.Wday-1, targetDay);
        break;
      }
      targetDay = now.Wday-1;
      if ( todaysTimeIsPast(now, alarm) ) {
        targetDay++;
      }
      if ( targetDay > SATURSDAY ) targetDay = SUNDAY;
      if ( targetDay > SUNDAY ) targetDay = SATURSDAY;
      dayToGo = getDayToGo(now.Wday-1, targetDay);
      break;
    }

    case ALL_DAYS: 
    {
      int targetDay = 0;
      targetDay = now.Wday-1;   
      if ( todaysTimeIsPast(now, alarm) ) {
        targetDay = targetDay +1;
      }    
      if ( targetDay > SATURSDAY ) targetDay = SUNDAY;
      dayToGo = getDayToGo(now.Wday-1, targetDay);
    }    
    break;    
  }

  int hourToGo = alarm.wHour - now.Hour;
  int minuteTogo = alarm.wMin - now.Minute;
  timer += dayToGo*SECS_PER_DAY;
  timer += hourToGo*SECS_PER_HOUR;
  timer += minuteTogo*SECS_PER_MIN;
  return timer;

}

int8_t WeeklyAlarm::getDayToGo(uint8_t today, uint8_t target) {
  if (target < today) return target + 7 - today;
  return target - today;
}

bool WeeklyAlarm::todaysTimeIsPast(TimeElements now, Alarm &alarm) {
  if ( now.Hour > alarm.wHour || (now.Hour == alarm.wHour && now.Minute >= alarm.wMin) ) return true;
  return false;
}


void WeeklyAlarm::handler(){// realtime monitoring if any alarm have to be triggered
  if  ( !(millis() >= (_lastAlarmCheck + 60000)) ){
    return;  // check every 1 minute
  }

  Alarm *alarmList = alarmHead;
  while (alarmList) {
    if ( now() >= alarmList->target ) { //if it is time
      if ( alarmList->almSwitch ) {
      if ( alarmList->callback ) alarmList->callback(alarmList->id); //do callback
      }
      alarmList->target = getTimer(*alarmList); //set the next time
      //printAlarm(Serial, alarmList);
    }
    alarmList = alarmList->nextAlarm;   
  }

  _lastAlarmCheck = millis();
}

void WeeklyAlarm::toggle(uint8_t id){
  Alarm *alarm = getAlarmById(id);
  alarm->almSwitch = !alarm->almSwitch;
}

String WeeklyAlarm::isOnOff(uint8_t id){
  Alarm *alarm = getAlarmById(id);
  if(!alarm->almSwitch) return "OFF";
  return "ON";
}

String WeeklyAlarm::weekType(uint8_t id){
  const String weekType[10] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Week", "Week end", "All days" };
  Alarm *alarm = getAlarmById(id);
  uint8_t wType = alarm->type;
  return weekType[wType];
}

uint8_t WeeklyAlarm::almHour(uint8_t id){
  Alarm *alarm = getAlarmById(id);
  return alarm->wHour;
}

uint8_t WeeklyAlarm::almMin(uint8_t id){
  Alarm *alarm = getAlarmById(id);
  return alarm->wMin;
}

void WeeklyAlarm::printAlarm(Stream &stream, Alarm *alarm){ //use weekType, almHour and almMin for troubleshoot and for JSON object for web server
  if(!alarm) {
    stream.println("No alarm on this id!!!");
    return;
  }
  stream.print("alarm type: ");
  stream.print(alarm->type);
  stream.print("=> ");
  const String weekType[10] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Week", "Week end", "All days" };
  stream.print(weekType[alarm->type]);
  stream.print(" ");
  stream.print(alarm->wHour);
  stream.print(":");
  stream.print(alarm->wMin);
  TimeElements t;
  breakTime(alarm->target, t);
  stream.print(" alarm will fire at ");
  stream.print(weekType[t.Wday-1]);
  stream.print(" ");
  stream.print(t.Day);
  stream.print(" ");
  stream.print(t.Hour);
  stream.print(":");
  stream.println(t.Minute);
}

void WeeklyAlarm::printAlarm(uint8_t id, Stream &stream){
  Alarm *alarm = getAlarmById(id);
  printAlarm(stream, alarm);
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
  Alarm *alarm = getAlarmById(id); 
  alarmBck["switch"] = alarm->almSwitch; //depricated
  alarmBck["type"] = alarm->type;
  alarmBck["hour"] = alarm->wHour;
  alarmBck["minute"] = alarm->wMin;
  alarmBck["callback"] = alarm->callback;
  return alarmBck;
  }

void WeeklyAlarm::restoreAlarm(int8_t id, JsonObject& alarmBck){
  Alarm *alarm = getAlarmById(id);
  alarm->almSwitch = alarmBck["switch"];  
  alarm->type = alarmBck["type"];
  alarm->wHour = alarmBck["hour"];
  alarm->wMin = alarmBck["minute"];
}

