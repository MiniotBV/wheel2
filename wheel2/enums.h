#ifndef ENUMS_H
#define ENUMS_H

#include <Arduino.h>


enum eStates {
  S_STOPPING,   // wait until needle is up and arm angle is 0
  S_HOMING,
  S_PARKING,    // somewhat move outside after homing
  S_HOME,       // do nothing
  
  S_HOMING_BEFORE_PLAYING,
  S_GOTO_RECORD_START,
  S_PLAY_TILL_END,
  S_PLAYING,

  S_PAUSE,
  S_GOTO_TRACK,
  S_SKIP_FORWARD,
  S_SKIP_REVERSE,
  S_RESUME_AFTER_SKIP,

  S_HOMING_BEFORE_CLEANING,
  S_NEEDLE_CLEAN,
  S_RECORD_CLEAN,
  S_CALIBRATE,
  S_HOMING_FAILED,
  S_BAD_ORIENTATION
}; // eStates


enum eErrors {
  E_NONE = 0,

  E_NEEDLE_MOVE_BACKWARDS = 2,  // needle move backwards (outside); this means that needle might not have reached record of bad tracking
  E_NEEDLE_DIDNT_MOVE = 3,      // needle hasn't moved for a long time; this means bad tracking or the needle is not on the record

  E_SPEED_UP_FAILED = 4,

  E_HOMING_FAILED = 5,          // counld not finish homing; this means that there might be an obstruction (cable), of the toothed track is unstable, or calibration is bad

  E_ARMANGLE_LIMIT_POS = 6,     // the arm is close to its limit; maybe stuck to home position or hard bump
  E_ARMANGLE_LIMIT_NEG = 7,     // the arm is close to its limit; maybe an obstruction outside of the carriage, or hard bump

  E_TO_MUCH_TRAVEL = 8,         // if the record is far off-center for tracking of a huge tracking error (hard bump) during tracking?
}; // eErrors


enum eTimeMode {
  TM_MICROS,
  TM_MILLIS,
  TM_SECS,
  TM_MINS
}; // eTimeMode


enum eCheckMode {
  CM_NONE,
  CM_COMMAND,
  CM_VALUE
}; // eCheckMode


enum eRpmMode {
  RPM_AUTO,
  RPM_33,
  RPM_45,
  RPM_78
}; // eRpmMode


#endif // ENUMS_H
