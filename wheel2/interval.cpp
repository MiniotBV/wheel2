#include "log.h"
#include "interval.h"
#include "pico/time.h"
#include <stdint.h>
#include "hardware/timer.h"

Interval::Interval(uint64_t interval, eTimeMode mode = TM_MILLIS) :
  _interval(interval),
  _mode(mode) {
  _timenowPrev = timenow();
} // Interval()

bool Interval::tick() {
  uint64_t now = timenow();
  if (now - _timenowPrev >= _interval) {
      _timenowPrevPrev = _timenowPrev;
      _timenowPrev += _interval;

      if (now - _timenowPrev >= _interval) {
        _timenowPrevPrev = _timenowPrev;
        _timenowPrev = now;
      }
      return true;
  }
  return false;
} // tick()

uint64_t Interval::duration() {
  return timenow() - _timenowPrev;
} // duration()

void Interval::reset() {
  _timenowPrev = timenow();
  _onetimeLatch = true;
} // reset()

uint64_t Interval::timenow() {
  if (_mode == TM_MILLIS) {
    return time_us_64()/1000; // millis();
  } else if (_mode == TM_MICROS) {
    return time_us_64(); // micros();
  } else {
    return 0;
  }
} // timenow()

void Interval::offset(uint64_t offst) {
  _timenowPrev += offst;
} // offset()

bool Interval::once() {
  if (_onetimeLatch && ((timenow() - _timenowPrev)  > _interval)) {
    _onetimeLatch = false;
    return true;
  }
  return false;
} // once()
