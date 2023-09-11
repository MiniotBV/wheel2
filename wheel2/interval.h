#ifndef INTERVAL_H
#define INTERVAL_H

#include <Arduino.h>
#include "enums.h"

class Interval {
  private:
    uint64_t _interval = 0;
    uint64_t _timenowPrev = 0;
    uint64_t _timenowPrevPrev = 0;
    eTimeMode _mode;
    uint64_t timenow();
    bool _onetimeLatch = true;
  public:
    Interval(uint64_t interval, eTimeMode mode);
    bool tick();
    uint64_t duration();
    void reset();
    void offset(uint64_t offst);
    bool once();
}; // Interval

#endif
