#include "WeeklyAlarm.h"

#include "Time.h"
//include Time this way if you using PlatformIo
//#include "../../Time/TimeLib.h"


AlarmNode::AlarmNode() : 
  dayEnable(0),
  nextAlarm(nullptr),
  target(0)
{}

bool AlarmNode::reset()
{
  if ( !isEnable() || 2 > dayEnable) return false; //dont set if alarm off or no days is active
  TimeElements now;
  breakTime( ::now(), now);
  TimeElements alrm;
  breakTime( target, alrm );
  if ( (dayEnable & 1<<now.Wday) && !todaysTimeIsPast(now, alrm) ) {
    alrm.Day = now.Day;
    target = makeTime( alrm );
    return true;
  }
  alrm.Day = now.Day + getDayToGo( now );
  target = makeTime( alrm );
  return true;
}

bool AlarmNode::isEnable()
{
  return dayEnable & 1<<ALARM_ENABLE_MASK;
}

int8_t AlarmNode::getDayToGo(TimeElements &now)
{
  struct size3Bits {
    uint8_t day:3;
  }week;
  uint8_t count = 1;
  week.day = now.Wday + 1;
  uint8_t flag = dayEnable & 0b11111110;
  while ( !(1<<week.day & flag) ) {
    if ( week.day == 0 ) {
      count--;
    }
    week.day++;
    count++;    
  }
  return count;
}

bool AlarmNode::todaysTimeIsPast(TimeElements &now, TimeElements &alrm) const
{
  if ( now.Hour > alrm.Hour ) return true;
  if ( now.Hour == alrm.Hour && now.Minute >= alrm.Minute) return true;
  return false;
}

///////////////////////////////////////
      /*  weekly Alarm main class   */
AlarmNode* WeeklyAlarm::alarmHead = nullptr;

WeeklyAlarm::WeeklyAlarm() : alarm(nullptr)
{}


void WeeklyAlarm::remove()
{
  if (!isAlarm()) return;
  AlarmNode* temp = alarmHead;
  AlarmNode* previous = nullptr;
  while ( temp != alarm ) {
    if ( ! temp->nextAlarm ) return;
    previous = temp;
    temp = temp->nextAlarm;
  }
  previous->nextAlarm = temp->nextAlarm;
  delete temp;
}


WeeklyAlarm& WeeklyAlarm::set(uint8_t hour, uint8_t min)
{
  if (!isAlarm()) return *this;
  TimeElements t;
  breakTime( now(), t );
  t.Hour = hour;
  t.Minute = min;
  alarm->target = makeTime(t); // now hour and minutes is store into target
  
  return *this;
}


WeeklyAlarm& WeeklyAlarm::dayEnable(timeDayOfWeek_t day)
{
  if (!isAlarm()) return *this;
  alarm->dayEnable |= (1<<day);
  return *this;
}


WeeklyAlarm& WeeklyAlarm::dayDisable(timeDayOfWeek_t day)
{
  if (!isAlarm()) return *this;
  alarm->dayEnable &= ~(1<<day);
  return *this;
}


WeeklyAlarm& WeeklyAlarm::dayToggle(timeDayOfWeek_t day)
{
  if (!isAlarm()) return *this;
  alarm->dayEnable ^= 1<<day;
  return *this;
}


void WeeklyAlarm::alarmOn()
{
  if (!isAlarm()) return;
  dayEnable( ALARM_ENABLE_MASK );
  if (!alarm->reset() ) alarmOff();
}


void WeeklyAlarm::alarmOff()
{
  if (!isAlarm()) return;
  dayDisable( ALARM_ENABLE_MASK );
}


void WeeklyAlarm::alarmToggle()
{
  if (!isAlarm()) return;
  dayToggle( ALARM_ENABLE_MASK );
  if ( alarm->isEnable() && !alarm->reset()) {
    alarmOff();;
  }
}


void WeeklyAlarm::handler()
{// realtime monitoring if any alarm have to be triggered
  if ( !alarmHead ) return;
  AlarmNode *alm = alarmHead;
  while (alm) {
    if ( now() >= alm->target ) { //if it is time
      if ( alm->isEnable() ) {
        alm->reset();
        alm->callback();        
      }
    }
    alm = alm->nextAlarm;   
  }
}

bool WeeklyAlarm::isEnable()
{
  if (!isAlarm()) return false;
  return alarm->isEnable();
}

bool WeeklyAlarm::isDayEnable(timeDayOfWeek_t day)
{ 
  if (!isAlarm()) return false;
  return alarm->dayEnable & (1<<day);
}

bool WeeklyAlarm::isAlarm()
{
  if( alarm ) return true;
  Serial.println( "ERROR: WeeklyAlarm --> No alarm have been added, run [ add(callable) ] first !!!");
  return false;
}


void WeeklyAlarm::prettyPrintTime(time_t time, Stream &stream)
{
  TimeElements t;
  breakTime(time, t);
  const String weekType[10] {"invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  prettyPrintClock(t.Hour, t.Minute, stream);
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

void WeeklyAlarm::prettyPrintAlarm(Stream &stream)
{
  if (!isAlarm()) return;
  prettyPrintTime(alarm->target, stream);
}

void WeeklyAlarm::prettyPrintClock(int hour, int minute, Stream &stream) {
  if (hour < 10) stream.print(" ");
  stream.print(hour);
  stream.print(":");
  if (minute < 10) stream.print("0");
  stream.print(minute);
}

String WeeklyAlarm::toJSON()
{
  if (!isAlarm()) return String();
  String json = "{\"Settings\":{\"Switch\":"; 
  if ( isEnable() ) json += "true,";
  else json += "false,";
  json += "\"Days Enabled\":{\"Sunday\":";
  if ( isDayEnable(dowSunday) ) json += "true,";
  else json += "false,";
  json += "\"Monday\":";
  if ( isDayEnable(dowMonday) ) json += "true,";
  else json += "false,";
  json += "\"Tuesday\":";
  if ( isDayEnable(dowTuesday) ) json += "true,";
  else json += "false,";
  json += "\"Wednesday\":";
  if ( isDayEnable(dowWednesday) ) json += "true,";
  else json += "false,";
  json += "\"Thursday\":";
  if ( isDayEnable(dowThursday) ) json += "true,";
  else json += "false,";
  json += "\"Friday\":";
  if ( isDayEnable(dowFriday) ) json += "true,";
  else json += "false,";
  json += "\"Saturday\":";
  if ( isDayEnable(dowSaturday) ) json += "true,";
  else json += "false";
  json += "},\"hour\":";
  TimeElements t;
  breakTime(alarm->target, t);
  json += t.Hour;
  json += ",\"minute\":";
  json += t.Minute;
  json += "}}";
  return json;
}
