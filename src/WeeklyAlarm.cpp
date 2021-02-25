#include <WeeklyAlarm.h>

///////////////////////////////////////

AlarmObj::AlarmObj() : 
  type(AlarmType::SUNDAY),
  almSwitch(false),
  wHour(0),
  wMin(0),
  nextAlarm(NULL),
  target(0)
{}

void AlarmObj::set(AlarmType _type, bool _almSwitch, int8_t _wHour, int8_t _wMin){
  type = _type;
  almSwitch = _almSwitch;
  wHour = _wHour;
  wMin = _wMin;
}
JsonObject& AlarmObj::getJSON(JsonBuffer& jsonBuffer) {
  JsonObject& alarmObj = jsonBuffer.createObject(); 
  alarmObj["type"] = weekTypeToString(); //need strigtype
  alarmObj["switch"] = isOnOff();  ///almSwitch; need is onoff
  alarmObj["hour"] = wHour;
  alarmObj["minute"] = wMin;
  //alarmObj.prettyPrintTo(Serial);
  return alarmObj;
  }

void AlarmObj::parseJSON(JsonObject& alarmObj) {
  //alarmObj.prettyPrintTo(Serial);
  const char* _switch = alarmObj["switch"];
  almSwitch = OnOffToBool( _switch );
  const char* _type = alarmObj["type"];
  type = stringToWeekType( _type );
  wHour = alarmObj["hour"];
  wMin = alarmObj["minute"];
}

const char* AlarmObj::weekTypeToString() {
  const char* weekType[] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Week", "Weekend", "All days" };
  return weekType[static_cast<uint8_t const>(type)];
}

AlarmType AlarmObj::stringToWeekType(const char* weekTypeInput) {
  const char* weekType[] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Week", "Weekend", "All days" };
  for(int8_t i = 0; i < 10; i++){
    if(strcmp(weekTypeInput, weekType[i]) == 0) {
      return AlarmType(i);
    }
  }
}

char const *AlarmObj::isOnOff() {
  if(!almSwitch) return "OFF";
  return "ON";
}

bool AlarmObj::OnOffToBool(const char* _switch) {
  if (strcmp(_switch,"ON") == 0) return true;
  return false;
}

void AlarmObj::toggle() {
  almSwitch = !almSwitch;
}

void AlarmObj::printTo(Stream &stream) {
  stream.print("alarm type: ");
  stream.print(weekTypeToString());
  prettyPrintClock(wHour, wMin, stream);
  TimeElements t;
  breakTime(target, t);
  stream.print(" alarm will fire at ");
  const String dayOfWeek[] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  stream.print(dayOfWeek[t.Wday-1]);
  stream.print(" ");
  stream.print(t.Day);
  stream.print(" ");
  prettyPrintClock(t.Hour, t.Minute, stream);
  stream.println();
}

void AlarmObj::prettyPrintClock(int hour, int minute, Stream &stream) {
  if (hour < 10) stream.print(" ");
  stream.print(hour);
  stream.print(":");
  if (minute < 10) stream.print("0");
  stream.print(minute);
}

void Alarm::setCallback(void (*_callback)()) {
  callback = _callback;
}

void Alarm::callbackHandler() {
  if ( callback ) callback();
}

void AlarmInt::setCallback(void(*_callback)(int), uint8_t _callbackValue) {
  callback = _callback;
  callbackValue = _callbackValue;
}

void AlarmInt::callbackHandler() {
  if ( callback ) callback(callbackValue);
}

/**/
///////////////////////////////////////
      /*  weekly Alarm main class   */

WeeklyAlarm::WeeklyAlarm() : alarmHead(NULL)
{}

void WeeklyAlarm::add(AlarmObj &alarm) {
  alarm.target = getTimer(alarm);
  if ( !alarmHead ) {
    alarmHead = &alarm;
    return;
  }
  AlarmObj *alm = alarmHead;
  while ( alm->nextAlarm ) alm = alm->nextAlarm;
  alm->nextAlarm = &alarm;
}

void WeeklyAlarm::remove(AlarmObj &alarm) {
  if ( alarmHead == &alarm ) { //supress the first element
    alarmHead = alarmHead->nextAlarm;
    return;
  }
  AlarmObj *alm = alarmHead;

  do {
    if ( alm->nextAlarm == &alarm ) {
      alm->nextAlarm = alm->nextAlarm->nextAlarm; //work even NULL
      return;
    }
    alm = alm->nextAlarm;
  } while (alm->nextAlarm);
}

time_t WeeklyAlarm::getTimer(AlarmObj &alarm) {
  TimeElements now;
  time_t timer = ::now();
  breakTime(timer,now);

  //weeklyAlarm enum weekdays start with 0 and timeLib with 1... have to offset it.
  int dayToGo = 0;
  //get how many day to go until the next alarm
  int8_t type = static_cast<uint8_t>(alarm.type) +1; //+1 to fit timeLib 
  switch(alarm.type) { 
    case AlarmType::SUNDAY:
    case AlarmType::MONDAY:
    case AlarmType::TUESDAY:
    case AlarmType::WEDNESDAY:
    case AlarmType::THURSDAY:
    case AlarmType::FRIDAY:
    case AlarmType::SATURDAY:
    if (type != now.Wday) {
      dayToGo = getDayToGo(now.Wday, type);
      break;
    }
    if ( todaysTimeIsPast(now, alarm) ) {
      dayToGo = 7;
    }
    break;

    case AlarmType::WEEK: 
    {
      int targetDay = 0;
      if ( now.Wday < dowMonday || now.Wday > dowFriday ) {
        targetDay = dowMonday;
        dayToGo = getDayToGo(now.Wday, targetDay);
        break;
      }
      if ( todaysTimeIsPast(now, alarm) ) {
        targetDay = now.Wday+1;
      } else {
        targetDay = now.Wday;
      } 
      if ( targetDay > dowFriday ) targetDay = dowMonday;
      dayToGo = getDayToGo(now.Wday, targetDay);
      break;
    }

    case AlarmType::WEEK_END: 
    {
      int targetDay = 0;
      if ( now.Wday <= dowMonday && now.Wday >= dowFriday ) {
        targetDay = dowSaturday;
        dayToGo = getDayToGo(now.Wday, targetDay);
        break;
      }
      targetDay = now.Wday;
      if ( todaysTimeIsPast(now, alarm) ) {
        targetDay++;
      }
      if ( targetDay > dowSaturday ) targetDay = dowSunday;
      if ( targetDay > dowSunday ) targetDay = dowSaturday;
      dayToGo = getDayToGo(now.Wday, targetDay);
      break;
    }

    case AlarmType::ALL_DAYS: 
    {
      int targetDay = 0;
      targetDay = now.Wday;   
      if ( todaysTimeIsPast(now, alarm) ) {
        targetDay++;
      }    
      if ( targetDay > dowSaturday ) targetDay = dowSunday;
      dayToGo = getDayToGo(now.Wday, targetDay);
    }    
    break;    
  }

  int hourToGo = alarm.wHour - now.Hour;
  int minuteTogo = alarm.wMin - now.Minute;
  timer += dayToGo*SECS_PER_DAY;
  timer += hourToGo*SECS_PER_HOUR;
  timer += minuteTogo*SECS_PER_MIN;
  //prettyPrintTime(timer, Serial);
  return timer;

}

int8_t WeeklyAlarm::getDayToGo(uint8_t today, uint8_t target) {
  if (target < today) return target + 7 - today;
  return target - today;
}

bool WeeklyAlarm::todaysTimeIsPast(TimeElements now, AlarmObj &alarm) {
  if ( now.Hour > alarm.wHour ) return true;
  if ( now.Hour == alarm.wHour && now.Minute >= alarm.wMin) return true;
  return false;
}


void WeeklyAlarm::handler(){// realtime monitoring if any alarm have to be triggered
  if  ( !(millis() >= (_lastAlarmCheck + 60000)) ){
    return;  // check every 1 minute
  }
  AlarmObj *alarmList = alarmHead;
  while (alarmList) {
    if ( now() >= alarmList->target ) { //if it is time
      if ( alarmList->almSwitch ) alarmList->callbackHandler();
      alarmList->target = getTimer(*alarmList); //set the next time
      //alarmList->printTo(Serial);
    }
    alarmList = alarmList->nextAlarm;   
  }

  _lastAlarmCheck = millis();
}

void WeeklyAlarm::prettyPrintTime(time_t time, Stream &stream) {
  TimeElements t;
  breakTime(time, t);
  const String weekType[10] {"invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  AlarmObj::prettyPrintClock(t.Hour, t.Minute, stream);
  stream.print(" ");
  stream.print(weekType[t.Wday]);
  stream.print(" ");
  stream.print(t.Day);
  stream.print("/");
  stream.print(t.Month); 
  stream.print("/");
  stream.print(t.Year+1970); 
  stream.println(); 
}
