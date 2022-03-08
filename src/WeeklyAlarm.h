/*
WeeklyAlarm is a library inspired by a programmable thermostat witch will trigger
an alarm each enabled day at a specific time.


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

#ifndef WeeklyAlarm_h
#define WeeklyAlarm_h

#include "Arduino.h"

#include "Time.h"
//include Time this way if you using PlatformIo
//#include "../../Time/TimeLib.h"

#define ALARM_ENABLE_MASK (timeDayOfWeek_t)0

template<class Callable>
class Alarm;
class WeeklyAlarm;

class AlarmNode {
  protected:
  AlarmNode();
  virtual ~AlarmNode() = default;
  friend class WeeklyAlarm;
  bool reset();
  bool isEnable();
  int8_t getDayToGo(TimeElements &now);
  bool todaysTimeIsPast(TimeElements &now, TimeElements &alrm) const; 
  uint8_t dayEnable;
  virtual void callback() = 0; 
  AlarmNode *nextAlarm;
  time_t target;
};

///////////////////////////////////////////

/**
 * @brief 
 * 
 * @tparam Callable 
 * could be : function pointer, functor or lambda with or without capture 
 */
template<class Callable>
class Alarm : public AlarmNode {
  Alarm (Callable callable) : AlarmNode(), callable(callable) {}
  ~Alarm() override = default;
  friend class WeeklyAlarm;
  Callable callable;
  void callback() override { callable(); }
};


//////////////////////////////////////////

/**
 * @brief  Day of week timer
 * 
 */
class WeeklyAlarm
{
public:
/**
 * @brief Construct a new Weekly Alarm object
 * 
 */
  WeeklyAlarm();
/**
 * @brief 
 * set the callback of the alarm
 * could be : function pointer, functor or lambda with or without capture
 * @tparam Callable 
 * @param callable 
 * @return WeeklyAlarm& 
 */
  template<class Callable>
  WeeklyAlarm& add(Callable callable) {
    alarm = new Alarm<Callable>(callable);
    if ( !alarmHead ) {
      alarmHead = alarm;
      return *this;
    }
    AlarmNode *temp = alarmHead;
    while ( temp->nextAlarm ) temp = temp->nextAlarm;
    temp->nextAlarm = alarm;
    return *this;
  }

/**
 * @brief 
 * set the time of day that alarm will be trigged
 * @param hour 
 * @param min
 * @return WeeklyAlarm& 
 */
  WeeklyAlarm& set(uint8_t hour, uint8_t min);

/**
 * @brief 
 * put the day of week (timeDayofWeek_t) the you want to enable
 * @param day 
 * @return WeeklyAlarm& 
 */
  WeeklyAlarm& dayEnable(timeDayOfWeek_t day);
    /**
 * @brief 
 * put all day of week (timeDayofWeek_t) the you want to enable
 * @param day 
 * @return WeeklyAlarm& 
 */
  template<typename ... Days>
  WeeklyAlarm& dayEnable(timeDayOfWeek_t day, Days ... days) {
    dayEnable(day);
    dayEnable(static_cast<timeDayOfWeek_t>(days)...);
    return *this;
  } 

/**
 * @brief 
 * put the day of week (timeDayofWeek_t) the you want to disable
 * @param day
 * @return WeeklyAlarm& 
 */
  WeeklyAlarm& dayDisable(timeDayOfWeek_t day);
  /**
 * @brief 
 * put all day of week (timeDayofWeek_t) the you want to disable
 * @param day
 * @return WeeklyAlarm& 
 */
  template<typename ... Days>
  WeeklyAlarm& dayDisable(timeDayOfWeek_t day, Days ... days) {
    dayDisable(day);
    dayDisable(static_cast<timeDayOfWeek_t>(days)...);
    return *this;
  } 
  
  /**
 * @brief 
 * put the day of week (timeDayofWeek_t) the you want to toggle on/off
 * @param day
 * @return WeeklyAlarm& 
 */
  WeeklyAlarm& dayToggle(timeDayOfWeek_t day);
    /**
 * @brief 
 * put all day of week (timeDayofWeek_t) the you want to toggle on/off
 * @param day 

 * @return WeeklyAlarm& 
 */
  template<typename ... Rest>
  WeeklyAlarm& dayToggle(timeDayOfWeek_t day, Rest ... rest ) { 
    dayToggle(day);
    dayToggle(static_cast<timeDayOfWeek_t>(rest)...);
    return *this;
  }

/**
 * @brief 
 * enable the alarm (days enable have to be set first)
 */
  void alarmOn();
  /**
   * @brief 
   * Disable the alarm, other parameter kept untouch
   */
  void alarmOff();
  /**
   * @brief 
   * Toggle On/Off the alarm
   */
  void alarmToggle();
  /**
   * @brief 
   * Remove the alarm, all parameters and callback will be deleted
   */
  void remove();
  /**
   * @brief 
   * Main handler, call the function every minute to monitor when a callback is to be trigged
   */
  static void handler();
  /**
   * @brief 
   * print nice format of time_t object
   * @param time
   * @param stream 
   * 
   */
  static void prettyPrintTime(time_t time, Stream &stream);
  /**
   * @brief 
   * print nice format of alarm setup
   * @param stream 
   * print nice format of alarm setup
   */
  void prettyPrintAlarm(Stream &stream);
  /**
   * @brief 
   * check if alarm is enable
   * @return true
   * @return false 
   */
  bool isEnable();
  /**
   * @brief 
   * check if a specific day is active on an the alarm
   * @param day 
   * 
   * @return true 
   * @return false 
   */
  bool isDayEnable(timeDayOfWeek_t day);

  /**
   * @brief Get the Days Enable object flag
   * bit 1 to 7 = dowSunday to dowSaturday, bit 0 is for alarm enable flag
   * @return uint8_t 
   */
  uint8_t getDaysEnable() const {
    return alarm->dayEnable;
  }

  /**
   * @brief Build a JSON string of the alarm settings
   * 
   * @return const char* 
   */
  String toJSON();

private:
  static AlarmNode *alarmHead;
  AlarmNode *alarm;
  static void prettyPrintClock(int hour, int minute, Stream &stream);
  bool isAlarm();

};
#endif
