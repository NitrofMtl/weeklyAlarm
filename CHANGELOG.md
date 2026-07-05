# Changelog

---

## [5.1.0] - 2026-07-03


### ADD:

-POSIX time lib support
-make linked list sorted for fast handling

---

##  [5.0.0] - 2022/03/07

### ADD:

  - Change callback from void* to template: argument support void*, functor and lambda with capture
  - Change weektype for individual days enable function
  - Days selection are made with Time.h timeDayOfWeek_t
  - Setup function have been split and change for WeeklyAlarm&, so the can be call daisy chain
  - General API have been refactor

### REMOVE:
  - Control by string have been removed
  - ArduinoJson dependency have been remove only an build-in JSON getAlarm remain

---