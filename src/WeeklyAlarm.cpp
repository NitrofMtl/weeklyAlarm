#include "WeeklyAlarm.h"


#ifdef __NEWLIB__
void breakTime(time_t t, tmElements_t &tm)
{
  struct tm *tm_info = gmtime(&t);
  if (!tm_info)
  return;

  // Arduino TimeLib compatible:
  // Year  = years since 1970
  // Month = 1..12
  // Wday  = Sunday=1 ... Saturday=7
  tm.Year   = tm_info->tm_year -70;
  tm.Month  = tm_info->tm_mon + 1;
  tm.Day    = tm_info->tm_mday;

  // Arduino convention:
  // Sunday=1 ... Saturday=7
  tm.Wday = tm_info->tm_wday + 1;

  tm.Hour   = tm_info->tm_hour;
  tm.Minute = tm_info->tm_min;
  tm.Second = tm_info->tm_sec;
}


time_t makeTime(const tmElements_t &e)
{
  struct tm tm_info{};

  tm_info.tm_year = e.Year + 70;
  tm_info.tm_mon  = e.Month - 1;
  tm_info.tm_mday = e.Day;
  tm_info.tm_hour = e.Hour;
  tm_info.tm_min  = e.Minute;
  tm_info.tm_sec  = e.Second;

  return mktime(&tm_info);
}
#endif //__NEWLIB__


AlarmNode::AlarmNode() : 
  dayEnable(0),
  nextAlarm(nullptr),
  target(0)
{}


bool AlarmNode::reset()
{
  if ( !isEnable() || 2 > dayEnable) return false; //dont set if alarm off or no days is active
  if (target == 0) return false;// time not set yet, cannot compute next occurrence
  tmElements_t tmNow;
  breakTime( ::now(), tmNow);
  tmElements_t alrm;
  breakTime(target, alrm);
  alrm.Second = 0; // reset seconds to 0, because we only set hour and minutes
  if ( (dayEnable & 1<<tmNow.Wday) && !todaysTimeIsPast(tmNow, alrm) ) {
    alrm.Day = tmNow.Day;
    target = makeTime(alrm);
    return true;
  }
  alrm.Day = tmNow.Day + getDayToGo(tmNow);
  target = makeTime(alrm);
  return true;
}


bool AlarmNode::isEnable()
{
  return dayEnable & 1<<ALARM_ENABLE_MASK;
}


int8_t AlarmNode::getDayToGo(tmElements_t &now)
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


bool AlarmNode::todaysTimeIsPast(tmElements_t &now, tmElements_t &alrm) const
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
  if (node->target < now()) return; // dont sort if target is in the past
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


/**
 * @brief Arm the alarm and insert it into the scheduler queue.
 *
 * The alarm target time must be configured before calling this function.
 * If the alarm is already present in the queue, it is repositioned according
 * to its current target time.
 *
 * @note This function does not calculate the next alarm occurrence.
 *       The caller is responsible for updating the target time.
 */
void WeeklyAlarm::arm()
{
  if (!alarm || !isEnable()) return;
  pop(alarm);
  sort(alarm);
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
  tmElements_t t;
  breakTime( now(), t );
  t.Hour = hour;
  t.Minute = min;
  alarm->target = makeTime(t); // now hour and minutes is store into target
  
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
  dayEnable( ALARM_ENABLE_MASK );
  if (alarm->reset() ) {
    sort(alarm);
  }
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

  time_t n = now();
    
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


void WeeklyAlarm::setSingleAlarm(time_t target)
{
  if (!alarm) return;
  alarm->target = target;
}


void WeeklyAlarm::prettyPrintTime(time_t time, Stream &stream)
{
  tmElements_t t;
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
  tmElements_t t;
  breakTime(alarm->target, t);
  json += t.Hour;
  json += ",\"minute\":";
  json += t.Minute;
  json += "}}";
  return json;
}
