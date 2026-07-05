#include "WeeklyAlarm.h"


AlarmNode::AlarmNode() : 
  dayEnable(0),
  nextAlarm(nullptr),
  target(0)
{}


bool AlarmNode::reset()
{
  if ( !isEnable() || 2 > dayEnable) return false; //dont set if alarm off or no days is active
  WA_TimeElements now;
  WA_BREAKTIME( ::WA_NOW(), now);
  WA_TimeElements alrm;
  WA_BREAKTIME( target, alrm );
  if ( (dayEnable & 1<<now.WA_WDAY) && !todaysTimeIsPast(now, alrm) ) {
    alrm.WA_DAY = now.WA_DAY;
    target = WA_MAKETIME( alrm );
    return true;
  }
  alrm.WA_DAY = now.WA_DAY + getDayToGo( now );
  target = WA_MAKETIME( alrm );
  return true;
}


bool AlarmNode::isEnable()
{
  return dayEnable & 1<<ALARM_ENABLE_MASK;
}


int8_t AlarmNode::getDayToGo(WA_TimeElements &now)
{
  struct size3Bits {
    uint8_t day:3;
  }week;
  uint8_t count = 1;
  week.day = now.WA_WDAY + 1;
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


bool AlarmNode::todaysTimeIsPast(WA_TimeElements &now, WA_TimeElements &alrm) const
{
  if ( now.WA_HOUR > alrm.WA_HOUR ) return true;
  if ( now.WA_HOUR == alrm.WA_HOUR && now.WA_MINUTE >= alrm.WA_MINUTE) return true;
  return false;
}


///////////////////////////////////////
      /*  weekly Alarm main class   */
AlarmNode* WeeklyAlarm::alarmHead = nullptr;


WeeklyAlarm::WeeklyAlarm() : alarm(nullptr)
{}


void WeeklyAlarm::pop(AlarmNode *node)
{
  if (!node || !alarmHead) return;
  if (alarmHead == node) {
      alarmHead = node->nextAlarm;
      node->nextAlarm = nullptr;
      return;
  }
  AlarmNode* temp = alarmHead;
  while (temp->nextAlarm && temp->nextAlarm != node) {
      temp = temp->nextAlarm;
  }
  if (temp->nextAlarm == node) {
      temp->nextAlarm = node->nextAlarm;
      node->nextAlarm = nullptr;
    }
}


void WeeklyAlarm::sort(AlarmNode *node)
{
  if (!node) return;
  node->nextAlarm = nullptr; // ensure clean insertion
  if (!alarmHead || node->target < alarmHead->target) {
      node->nextAlarm = alarmHead;
      alarmHead = node;
      return;
  }
  AlarmNode* temp = alarmHead;
  while (temp->nextAlarm && temp->nextAlarm->target <= node->target) {
      temp = temp->nextAlarm;
  }
  node->nextAlarm = temp->nextAlarm;
  temp->nextAlarm = node;
}


void WeeklyAlarm::remove()
{
  if (!alarm) return;
  pop(alarm);
  delete alarm;
  alarm = nullptr;
}


WeeklyAlarm& WeeklyAlarm::set(uint8_t hour, uint8_t min)
{
  if (!alarm) return *this;
  WA_TimeElements t;
  WA_BREAKTIME( WA_NOW(), t );
  t.WA_HOUR = hour;
  t.WA_MINUTE = min;
  alarm->target = WA_MAKETIME(t); // now hour and minutes is store into target
  
  return *this;
}


WeeklyAlarm& WeeklyAlarm::dayEnable(timeDayOfWeek_t day)
{
  if (!alarm) return *this;
  alarm->dayEnable |= (1<<day);
  return *this;
}


WeeklyAlarm& WeeklyAlarm::dayDisable(timeDayOfWeek_t day)
{
  if (!alarm) return *this;
  alarm->dayEnable &= ~(1<<day);
  return *this;
}


WeeklyAlarm& WeeklyAlarm::dayToggle(timeDayOfWeek_t day)
{
  if (!alarm) return *this;
  alarm->dayEnable ^= 1<<day;
  return *this;
}


void WeeklyAlarm::alarmOn()
{
  if (!alarm) return;
  if (isEnable()) return;
  if (alarm->target == 0) {
    Serial.println("ERROR: WeeklyAlarm --> set() must be called before alarmOn()");
    return;
  }
  dayEnable( ALARM_ENABLE_MASK );
  if (!alarm->reset() ) {
    alarmOff();
  }
  sort(alarm);
}


void WeeklyAlarm::alarmOff()
{
  if (!alarm) return;
  if (!isEnable()) return;
  dayDisable( ALARM_ENABLE_MASK );
  pop(alarm);
}


void WeeklyAlarm::alarmToggle()
{
  if (!alarm) return;
  if (isEnable()) {
    alarmOff();
    return;
  }
  alarmOn();
}


void WeeklyAlarm::handler()
{
  if ( !alarmHead ) return;

  time_t n = WA_NOW();
    
  while (alarmHead && n >= alarmHead->target) {
    AlarmNode* alm = alarmHead;
    
    if (alm->isEnable()) {
        pop(alm);           // remove from list first
        alm->callback();
        alm->reset();       // compute next target
        sort(alm);          // re-insert at correct position
    } else {
        alm = alm->nextAlarm; // skip disabled, shouldn't be in list but safety
    }
  }
}


bool WeeklyAlarm::isEnable()
{
  if (!alarm) return false;
  return alarm->isEnable();
}


bool WeeklyAlarm::isDayEnable(timeDayOfWeek_t day)
{ 
  if (!alarm) return false;
  return alarm->dayEnable & (1<<day);
}


void WeeklyAlarm::prettyPrintTime(time_t time, Stream &stream)
{
  WA_TimeElements t;
  WA_BREAKTIME(time, t);
  const String weekType[10] {"invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  prettyPrintClock(t.WA_HOUR, t.WA_MINUTE, stream);
  stream.print(" ");
  stream.print(weekType[t.WA_WDAY]);
  stream.print(" ");
  stream.print(t.WA_DAY);
  stream.print("/");
  stream.print(t.WA_MONTH); 
  stream.print("/");
  stream.print(t.WA_YEAR+1970); 
  stream.println(); 
}


void WeeklyAlarm::prettyPrintAlarm(Stream &stream)
{
  if (!alarm) return;
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
  if (!alarm) return String();
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
  WA_TimeElements t;
  WA_BREAKTIME(alarm->target, t);
  json += t.WA_HOUR;
  json += ",\"minute\":";
  json += t.WA_MINUTE;
  json += "}}";
  return json;
}
