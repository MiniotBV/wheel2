#include "log.h"
#include "shared.h"
#include "helper.h"


Shared::Shared(int appversion, String appdate) :
  appversion(appversion),
  appdate(appdate),
  stateChangedInterval(1000, TM_MILLIS),
  errorChangedInterval(0, TM_MILLIS) {
} // Shared()


void Shared::setState(eStates newState) {
  LOG_DEBUG("shared.cpp", "[setState]");
  stateChangedInterval.reset();
  firstTimeStateChange = true;

  state = newState;
  // LOG_DEBUG("shared.cpp", "[setState] State changed to " + getState(state));
  Serial.println("STATE: " + getState(state));
} // setState()


void Shared::setError(eErrors newError) {
  LOG_DEBUG("shared.cpp", "[setError]");
  error = newError;
  errorChangedInterval.reset();
  Serial.println("ERROR: " + getError(error));
  Serial.println("STATE: " + getState(state));
  errorCount[newError] += 1;
} // setError()


bool Shared::firstTimeStateChanged() {
  if (firstTimeStateChange) {
    firstTimeStateChange = false;
    return true;
  }
  return false;
} // firstTimeStateChanged()

void Shared::info() {
  for (int error = 0; error < E_MAX; error++) {
    String err = getError(static_cast<eErrors>(error));
    if ((err != "E_UNKNOWN") && (err != "E_NONE")) {
      Serial.println(padRight(err, PADR) + ": " + String(errorCount[error]));
    }
  }

  Serial.println();
} // info()
