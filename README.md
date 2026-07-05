# weeklyAlarm

## **weekday timer ( thermostat like scheduler )**


**WeeklyAlarm is a timer to trigger daily at the same time. It has been designed to be used like a numeric thermostat scheduler.**

## dependency:


  - [Arduino time library](https://github.com/PaulStoffregen/Time)

  **Only with board taht dont include POSIX time




  ### **Created on 03/09/2018**
  ### **By Nitrof**


# WeeklyAlarm

A lightweight Arduino library for scheduling weekly recurring alarms, inspired by programmable thermostats. Each alarm fires on selected days of the week at a specific time, with support for multiple callbacks, lambdas, and functors.

---

## Platform Support

| Platform | Time Source |
|----------|------------|
| STM32 / newlib (Opta, Portenta, etc.) | POSIX `time.h` — use `set_time(epoch)` to set time |
| AVR / classic Arduino | [Paul Stoffregen's Time library](https://github.com/PaulStoffregen/Time) — use `setTime(epoch)` |

The correct time backend is selected automatically at compile time via `__NEWLIB__` detection. No configuration needed.

---

## Installation

### PlatformIO
```ini
lib_deps =
    WeeklyAlarm
    # Only needed on non-newlib platforms (AVR, etc.):
    paulstoffregen/Time
```

### Arduino IDE
Download and install via Sketch → Include Library → Add .ZIP Library.

---

## Basic Usage

```cpp
#include "WeeklyAlarm.h"

WeeklyAlarm myAlarm;

void onAlarm() {
    Serial.println("Alarm fired!");
}

void setup() {
    Serial.begin(9600);
    set_time(epoch); // or setTime(epoch) on AVR — set your RTC/NTP time first

    myAlarm
        .add(onAlarm)
        .set(7, 30)                              // 07:30
        .dayEnable(dowMonday, dowWednesday, dowFriday)
        .alarmOn();
}

void loop() {
    WeeklyAlarm::handler();
}
```

---

## API Reference

### Construction

```cpp
WeeklyAlarm myAlarm;
```

Creates a new alarm instance. No alarm is active until `add()`, `set()`, `dayEnable()`, and `alarmOn()` are called.

---

### `add(callable)` → `WeeklyAlarm&`

Attaches a callback to the alarm. Must be called before `set()` or `alarmOn()`.

Accepts any callable: function pointer, functor, or lambda (with or without capture).

```cpp
// Function pointer
myAlarm.add(myFunction);

// Lambda without capture
myAlarm.add([]() { digitalWrite(LED, HIGH); });

// Lambda with capture
int value = 42;
myAlarm.add([value]() { Serial.println(value); });
```

Returns `*this` for chaining.

---

### `set(hour, min)` → `WeeklyAlarm&`

Sets the time of day the alarm will fire.

```cpp
myAlarm.set(7, 30);   // 07:30
myAlarm.set(22, 0);   // 22:00
```

Must be called after `add()` and before `alarmOn()`. Returns `*this` for chaining.

---

### `dayEnable(day, ...)` → `WeeklyAlarm&`

Enables one or more days of the week for this alarm.

```cpp
myAlarm.dayEnable(dowMonday);
myAlarm.dayEnable(dowMonday, dowWednesday, dowFriday);
myAlarm.dayEnable(dowSaturday, dowSunday);
```

Available day constants:

| Constant | Value |
|----------|-------|
| `dowSunday` | 1 |
| `dowMonday` | 2 |
| `dowTuesday` | 3 |
| `dowWednesday` | 4 |
| `dowThursday` | 5 |
| `dowFriday` | 6 |
| `dowSaturday` | 7 |

Returns `*this` for chaining.

---

### `dayDisable(day, ...)` → `WeeklyAlarm&`

Disables one or more days of the week.

```cpp
myAlarm.dayDisable(dowSaturday, dowSunday);
```

Returns `*this` for chaining.

---

### `dayToggle(day, ...)` → `WeeklyAlarm&`

Toggles one or more days on/off.

```cpp
myAlarm.dayToggle(dowMonday);
```

Returns `*this` for chaining.

---

### `alarmOn()`

Enables the alarm and inserts it into the sorted active alarm list. `set()` and at least one `dayEnable()` must be called first.

```cpp
myAlarm.alarmOn();
```

Calling `alarmOn()` before `set()` will print an error and return without enabling.

---

### `alarmOff()`

Disables the alarm and removes it from the active list. All day and time settings are preserved.

```cpp
myAlarm.alarmOff();
```

---

### `alarmToggle()`

Toggles the alarm on or off.

```cpp
myAlarm.alarmToggle();
```

---

### `remove()`

Removes the alarm from the active list and frees its memory. The `WeeklyAlarm` instance can be reused by calling `add()` again.

```cpp
myAlarm.remove();
```

---

### `isEnable()` → `bool`

Returns `true` if the alarm is currently enabled.

```cpp
if (myAlarm.isEnable()) { ... }
```

---

### `isDayEnable(day)` → `bool`

Returns `true` if the specified day is enabled for this alarm.

```cpp
if (myAlarm.isDayEnable(dowMonday)) { ... }
```

---

### `getDaysEnable()` → `uint8_t`

Returns the raw day-enable bitmask. Bits 1–7 correspond to Sunday–Saturday. Bit 0 is the alarm enable flag.

```cpp
uint8_t mask = myAlarm.getDaysEnable();
```

---
### `getTime()` → `WA_TimeElements`

Returns the alarm's scheduled time of day as a `WA_TimeElements` structure.

```cpp
WA_TimeElements t = myAlarm.getTime();
uint8_t hour   = t.WA_HOUR;
uint8_t minute = t.WA_MINUTE;
```

`WA_TimeElements` is a platform-transparent typedef — `struct tm` on newlib/STM32, `TimeElements` on AVR/TimeLib. Use the `WA_` field macros for portable member access.

---

### `toJSON()` → `String`

Returns a JSON string representation of the alarm's current settings.

```cpp
Serial.println(myAlarm.toJSON());
// {"Settings":{"Switch":true,"Days Enabled":{"Sunday":false,"Monday":true,...},"hour":7,"minute":30}}
```

---

### `prettyPrintAlarm(stream)`

Prints a human-readable description of the alarm's next scheduled time to a `Stream` (Serial, etc.).

```cpp
myAlarm.prettyPrintAlarm(Serial);
// 07:30 Monday 1/1/2025
```

---

### `static handler()`

Main dispatch function. Call this from `loop()` — it is self-throttling via the sorted alarm list and has negligible overhead when no alarm is due.

```cpp
void loop() {
    WeeklyAlarm::handler();
}
```

Internally, alarms are maintained in a sorted linked list ordered by next fire time. The handler checks only the head of the list — if the head is not due, it returns immediately (O(1)). When an alarm fires, its callback is called, its next target time is computed, and it is re-inserted at the correct sorted position. Multiple alarms due at the same time are each fired on successive `loop()` passes to avoid stalling other tasks.

---

### `static prettyPrintTime(time, stream)`

Prints a human-readable time_t value to a Stream.

```cpp
WeeklyAlarm::prettyPrintTime(time_t, Serial);
// 07:30 Monday 1/1/2025
```

---

## Chaining Example

```cpp
myAlarm
    .add([]() { setHeating(21.0f); })
    .set(7, 0)
    .dayEnable(dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday)
    .alarmOn();
```

---

## Multiple Alarms

Each `WeeklyAlarm` instance manages one time-value pair. Use multiple instances for complex schedules:

```cpp
WeeklyAlarm morningAlarm;
WeeklyAlarm eveningAlarm;
WeeklyAlarm weekendAlarm;

morningAlarm.add([]() { setpoint(21.0f); }).set(7, 0).dayEnable(dowMonday, dowFriday).alarmOn();
eveningAlarm.add([]() { setpoint(18.0f); }).set(18, 0).dayEnable(dowMonday, dowFriday).alarmOn();
weekendAlarm.add([]() { setpoint(20.0f); }).set(9, 0).dayEnable(dowSaturday, dowSunday).alarmOn();
```

All instances share the same sorted list and are dispatched by the single `WeeklyAlarm::handler()` call.

---

## Notes

- Time must be set (via RTC, NTP, or BACnet TimeSynchronization) before calling `alarmOn()`.
- Alarm resolution is one minute — sub-minute precision is not supported.
- The library does not manage time synchronization. Use `set_time()` (STM32) or `setTime()` (AVR/TimeLib) from your sketch or a sync provider.
- On STM32 platforms, the POSIX `timeDayOfWeek_t` enum is provided by the library. On other platforms it comes from TimeLib.

---

## License

MIT License — see source file header for full text.