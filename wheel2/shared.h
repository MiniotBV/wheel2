#ifndef SHARED_H
#define SHARED_H

#include <Arduino.h>
#include "enums.h"
#include "interval.h"


class Shared {
  private:
    bool firstTimeStateChange = false;
  public:
    Interval stateChangedInterval;
    Interval errorChangedInterval;
    int appversion;
    String appdate;
    eStates state = S_HOME;
    eErrors error = E_NONE;
    bool puristMode = false;
    Shared(int appversion, String appdate);
    void setState(eStates state);
    void setError(eErrors newError);
    bool firstTimeStateChanged();
}; // Shared


#endif //SHARED_H
