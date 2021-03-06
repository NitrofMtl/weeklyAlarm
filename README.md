# weeklyAlarm V4.0
## **weekday timer ( thermostat like scheduler )**


**WeeklyAlarm is a timer to trigger daily at the same time. It has been designed to be used like a numeric thermostat scheduler.**

## dependency:

  - [Arduino time library](https://github.com/PaulStoffregen/Time)

  - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

## New on version 4.0
  - Strong typing for alarm type
  - now support building 2 kind of callback (*void)() and (*void)(int)
  - Enable inheritance with AlarmObj to create custom callback
  - better ArduinoJSON integration
  - general simper utilization:

  ###  If functionality is pretty the same as v3.0 and previous, usage is not the same and is not directly compatible. See the example to see change.


###  New on version 3.0:
  - Remove linkedList dependency.
  - Handler run with time_t object, faster execution.
  - Add print alarm by stream object reference:
  

  ### **Created on 03/09/18**
  ### **By Nitrof**

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