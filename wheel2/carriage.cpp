#include "log.h"
#include "carriage.h"
#include "helper.h"
#include "pins.h"
#include "pwm.h"


Carriage::Carriage(Shared& shared, Arm& arm, Plateau& plateau, Scanner& scanner) :
  _shared(shared),
  _arm(arm),
  _plateau(plateau),
  _scanner(scanner),
  _interval(1000, TM_MICROS),
  movedForwardInterval(1, TM_MILLIS) {
} // Carriage()


void Carriage::init(SpeedComp* speedcomp) { // to prevent circular reference
  LOG_DEBUG("carriage.cpp", "[init]");
  _speedcomp = speedcomp;
  setPwm(CARRIAGE_STEPPER_AP_PIN);
  setPwm(CARRIAGE_STEPPER_AN_PIN);
  setPwm(CARRIAGE_STEPPER_BP_PIN);
  setPwm(CARRIAGE_STEPPER_BN_PIN);
} // init()


void Carriage::func() {
  if (_interval.tick()) {
    //--------------------------------------------- ARM ANGLE
    _arm.armAngleRaw += (analogRead(ARM_ANGLE_SENSOR_PIN) - _arm.armAngleRaw ) / 6;

    if (millisSinceBoot() > 1000) {
        if (_arm.armAngleRaw < _arm.armAngleMinCall) {
          _arm.armAngleMinCall = _arm.armAngleRaw;
        }
        if (_arm.armAngleRaw > _arm.armAngleMaxCall) {
          _arm.armAngleMaxCall = _arm.armAngleRaw;
        }
    }
    _arm.armAngleCall = mapFloat(_arm.armAngleRaw, _arm.armAngleMin, _arm.armAngleMax, 1, -1);
    _arm.armAngleDiff = _arm.armAngleCall - _arm.armAnglePrev;
    _arm.armAnglePrev = _arm.armAngleCall;
    _arm.armAngleSlow += (_arm.armAngleCall - _arm.armAngleSlow) / 100;
    _arm.armAngle = _arm.armAngleCall - _arm.armAngleOffset;

    //--------------------------------------------- LIMIT ERROR
    if (_shared.state == S_PLAYING) {
      if (_arm.armAngleCall > 0.95) {
        _shared.setError(E_ARMANGLE_LIMIT_POS);
        _arm.needleUp();
        _shared.setState(S_HOME);
      }
      if (_arm.armAngleCall < -0.95) {
        _shared.setError(E_ARMANGLE_LIMIT_NEG);
        _arm.needleUp();
        _shared.setState(S_HOME);
      }
    }

    sensorPosition = (position - CARRIAGE_SENSOR_OFFSET) - trackOffset;

    stateUpdate();

    _Dcomp *= 0.999;
    _Dcomp += limitFloat(_arm.armAngleDiff * D, -CARRIAGE_MAX_SPEED, CARRIAGE_MAX_SPEED); // to prevent oscillation
    realPosition = position + _Dcomp;

    if (offCenterCompensation) {
      _offCenterCompFilter +=  (_speedcomp->carriageFourierFilter - _offCenterCompFilter) / 4;
      realPosition += _offCenterCompFilter;
    }

    _motorPos = (realPosition + _offset) * _mm2step;

    if (_motorEnable) {
      pwmStepper(-_motorPos, CARRIAGE_STEPPER_AP_PIN, CARRIAGE_STEPPER_AN_PIN, CARRIAGE_STEPPER_BP_PIN, CARRIAGE_STEPPER_BN_PIN, true);
    } else {
      pwmDisableStepper(CARRIAGE_STEPPER_AP_PIN, CARRIAGE_STEPPER_AN_PIN, CARRIAGE_STEPPER_BP_PIN, CARRIAGE_STEPPER_BN_PIN);
    }

    if (_shared.state == S_PLAYING && _arm.isNeedleDown()) { // carriage pos filter for display
      float div = position - positionFilter;
      if (div < 0) {
        positionFilter += (div) / 1000;
      }
    } else {
      positionFilter = position;
    }

    _motorEnable = true;

    if (graphicData) {
      printGraphicData();
    } else {
      _headerShown = false;
    }
  } // _interval.tick()
} // func()


void Carriage::stateUpdate() {
  if (_shared.state == S_HOME) {
    _arm.centerArmAngle();
    _motorEnable = false;
    repeat = false;
    return;
  }

  if (_shared.state == S_STOPPING) {
    if (_arm.needleUp() && decelerate()) {
      _shared.setState(S_HOMING);
    }
    return;
  }


  if (_shared.state == S_HOMING || _shared.state == S_HOMING_BEFORE_PLAYING || _shared.state == S_HOMING_BEFORE_CLEANING) {
    // wait for stepper to stop shaking after turning on
    if (_shared.stateChangedInterval.duration() < 300) {
      return;
    }

    float speed = mapFloat(_arm.armAngleCall, 0.75, 0.5, 0, CARRIAGE_MAX_SPEED);
    speed = limitFloat(speed, (CARRIAGE_MAX_SPEED / 10), CARRIAGE_MAX_SPEED);
    bool arrived = movetoPosition(-150, speed);
    // bool arrived = movetoPosition(-150, CARRIAGE_MAX_SPEED);

    if (arrived) {
      _offset -= CARRIAGE_PARK - position;
      position = CARRIAGE_PARK;
      
      if (_shared.error == E_HOMING_FAILED){
        _shared.setError(E_HOMING_FAILED);
        _shared.setState(S_PARKING);
      } else {
        _shared.setError(E_HOMING_FAILED);
        _shared.setState(S_HOMING_FAILED);
      }
      return;
    }

    if (_arm.armAngleCall > 0.75) { //75 //-800 //-1000
      // LOG_DEBUG("carriage.cpp", "[stateUpdate] Home diff: " + String(CARRIAGE_PARK - realPosition) + " realPosition: " + String(realPosition) + " CARRIAGE_PARK: " + String(CARRIAGE_PARK) + " Dcomp: " + String(_Dcomp));
      LOG_DEBUG("carriage.cpp", "[stateUpdate] Home diff: " + String(CARRIAGE_PARK - realPosition) + " realPosition: " + String(realPosition));
      // Serial.println("Home diff: " + String(CARRIAGE_PARK - realPosition) + " realPosition: " + String(realPosition));
      
      _offset -= CARRIAGE_PARK - realPosition;
      position = CARRIAGE_PARK;
      _Dcomp = 0;

      emergencyStop();
      _speedcomp->clearCompSamples(); // nice moment to stop

      if (_shared.state == S_HOMING_BEFORE_PLAYING) {
        _shared.setState(S_GOTO_RECORD_START);
      } else if (_shared.state == S_HOMING_BEFORE_CLEANING) {
        _shared.setState(S_NEEDLE_CLEAN);
      } else {
        _shared.setState(S_PARKING);
      }
      return;
    }
    return;
  }


  if (_shared.state == S_PARKING) {
    if (movetoPosition(CARRIAGE_HOME, CARRIAGE_MAX_SPEED)) {
      // if (_shared.stateChangedInterval.duration() < 2000) {
        _shared.setState(S_HOME);
      // }
    }
    _Dcomp = 0;
    // _Dcomp *= 0.98;
    return;
  }


  if (_shared.state == S_HOMING_FAILED) {
    if (movetoPosition(CARRIAGE_HOME + 10, CARRIAGE_MAX_SPEED)) {
      if (_shared.stateChangedInterval.duration() < 2000) {
        _arm.centerArmAngle();
        return;
      }
      _shared.setState(S_HOMING);
    } 
    return;
  }


  if (_shared.state == S_BAD_ORIENTATION) {
    _arm.needleUp();
    _motorEnable = false;
    return;
  }


  if (_shared.state == S_GOTO_RECORD_START) {
    if (_shared.firstTimeStateChanged()) {
      _scanner.recordStart = 1000;
    }

    if (!_scanner.recordPresent && sensorPosition > CARRIAGE_RECORD_END + 1) { // record present?
      float recordDiaInch = (sensorPosition / 25.4) * 2;

      if (recordDiaInch < 6) { // stop when smaller than 6"
        // LOG_DEBUG("carriage.cpp", "[stateUpdate] No record? RecordDiameter: " + String(recordDiaInch));
        Serial.println("No record? RecordDiameter: " + String(recordDiaInch));
        _plateau.stop();
        return;
      } else if (recordDiaInch < 9) {
        // LOG_DEBUG("carriage.cpp", "[stateUpdate] RecordDiameter: " + String(recordDiaInch) + " : ±7\" ");
        Serial.println("RecordDiameter: " + String(recordDiaInch) + " : ±7\" ");
        _plateau.setRpm(RPM_45);
        _scanner.recordStart = CARRIAGE_7INCH_START;
        _scanner.setTracksAs7inch();
      } else if (recordDiaInch < 11) { 
        // LOG_DEBUG("carriage.cpp", "[stateUpdate] RecordDiameter: " + String(recordDiaInch) + " : ±10\" ");
        Serial.println("RecordDiameter: " + String(recordDiaInch) + " : ±10\" ");
        _plateau.setRpm(RPM_33);
        _scanner.recordStart = CARRIAGE_10INCH_START;
        _scanner.check();
      } else {
        // LOG_DEBUG("carriage.cpp", "[stateUpdate] RecordDiameter: " + String(recordDiaInch) + " : ???\" ");
        Serial.println("RecordDiameter: " + String(recordDiaInch) + " : ???\" ");
        _scanner.recordStart = sensorPosition;
        // _plateau.setRpm(RPM_33);
      }
      targetTrack = _scanner.recordStart;
      _shared.setState(S_PLAY_TILL_END);
      return;
    }

    // when arrived at carriage endrange
    if (movetoPosition(CARRIAGE_12INCH_START, CARRIAGE_MAX_SPEED)) { 
      _scanner.recordStart = CARRIAGE_12INCH_START;
      targetTrack = _scanner.recordStart;
      // LOG_DEBUG("carriage.cpp", "[stateUpdate] RecordDiameter: 12\" ");
      Serial.println("RecordDiameter: 12\" ");
      _plateau.setRpm(RPM_33);
      _scanner.check();

      _shared.setState(S_PLAYING);
      return;
    }
    return;
  }

  if (_shared.state == S_PLAY_TILL_END) {
    if (movetoPosition(targetTrack, CARRIAGE_MAX_SPEED)) {
      _shared.setState(S_PLAYING);
      return;
    }
    return;
  }


  //  ================================================================
  //      PLAY
  //  ================================================================
  if (_shared.state == S_PLAYING) {
    if (_shared.stateChangedInterval.duration() < 1000) {
      _arm.centerArmAngle();
      movedForwardInterval.reset();
      return;
    }

    // if (_arm.isNeedleDownFor(3000)) { // error 3 fix i think
    //   movedForwardInterval.reset(); // reset timer to prevent error 3
    // }

    if (_arm.needleDown()) {
      //---------------------------------------- transport calculations
      _newPosition = position + limitFloat(_arm.armAngle * P, -3, 3);
      _newPosition = limitFloat(_newPosition, 0, _scanner.recordStart);
      movetoPosition(_newPosition, CARRIAGE_MAX_SPEED);
      
      //---------------------------------------- events during playing
      if (realPosition <= CARRIAGE_RECORD_END) {
        // LOG_NOTICE("carriage.cpp", "[stateUpdate] Carriage reached limit!");
        Serial.println("Carriage reached limit!");
        stopOrRepeat();
        return;
      }
      if (realPosition < positionFilter - 3) {
        // LOG_NOTICE("carriage.cpp", "[stateUpdate] Run-out groove?");
        Serial.println("Run-out groove?");
        stopOrRepeat();
        return;
      }
      if (realPosition > positionFilter + 2.5) {
        _shared.setError(E_NEEDLE_MOVE_BACKWARDS);
        _plateau.stop();
        return;
      }

      if (_speedcomp->trackSpacing > 0.01) {
        movedForwardInterval.reset();
      } else if (movedForwardInterval.duration() > 4000) {
        if (position < 60 ){ // run-out groove? (54mm seems the farest from the middle
          stopOrRepeat();
          return;
        } else {
          _shared.setError(E_NEEDLE_DIDNT_MOVE); // carriage didn't move for a while
          movedForwardInterval.reset(); // reset time to prevent another trigger
          gotoTrack(position - 0.25); // move carriage 0.5mm inside to skip the skip
          return;
        }
      } 

      if (_shared.puristMode) {
        if ((_speedcomp->wow < 0.15) || _arm.isNeedleDownFor(10000) ){
          // LOG_NOTICE("carriage.cpp", "[stateUpdate] Seems to runs ok");
          Serial.println("Seems to runs ok");
          _shared.puristMode = false;
          Serial.println("PURIST MODE: OFF");
        // gotoRecordStart();
          gotoTrack(targetTrack);
        }
      }    
    }
    return;
  }


  //  ================================================================
  //      TRACKS & SKIPPING
  //  ================================================================
  if (_shared.state == S_GOTO_TRACK) {
    if (_arm.needleUp()) {
      if (movetoPosition(targetTrack, CARRIAGE_MAX_SPEED)) {
        _shared.setState(S_PLAYING);
        return;
      }
    }
    return;
  }

  if (_shared.state == S_SKIP_FORWARD) {
    if (_arm.needleUp()) {
      movetoPosition(CARRIAGE_RECORD_END, CARRIAGE_MAX_SPEED / 4);
    }
    targetTrack = position; // to clean display
  }

  if (_shared.state == S_SKIP_REVERSE) {
    if (_arm.needleUp()) {
      movetoPosition(_scanner.recordStart, CARRIAGE_MAX_SPEED / 4);
    }
    targetTrack = position; // to clean display
  }

  if (_shared.state == S_RESUME_AFTER_SKIP) {
    if (movetoPosition(targetTrack, CARRIAGE_MAX_SPEED / 4)) { 
      _shared.setState(S_PLAYING);
      return;
    }
    return;
  }

  if (_shared.state == S_PAUSE) {
    if (_arm.needleUp()) {
      movetoPosition(targetTrack, CARRIAGE_MAX_SPEED);
    }
    return;
  }

  if (_shared.state == S_NEEDLE_CLEAN) {
    if (movetoPosition(CARRIAGE_CLEAN_POS, CARRIAGE_MAX_SPEED)) {
      _arm.needleDown();
    }

    if (sensorPosition > CARRIAGE_RECORD_END + 2 && sensorPosition < CARRIAGE_RECORD_END + 12 && _scanner.recordPresent) {
      // record present? stop!
      // LOG_ALERT("carriage.cpp", "[stateUpdate] Cannot clean needle; Record present?");
      Serial.println("Cannot clean needle; Record present? Clean record instead");
      _shared.setState(S_RECORD_CLEAN);
      _plateau.motorStart();
      _plateau.setRpm(RPM_33);
    }
    return;
  }

  if (_shared.state == S_RECORD_CLEAN) {
    decelerate();
    if (!_scanner.recordPresent) {
      // LOG_ALERT("carriage.cpp", "[stateUpdate] Record removed!");
      Serial.println("Record removed!");
      _plateau.stop();
    }
    return;
  }

  if (_shared.state == S_CALIBRATE) {
    if (_shared.stateChangedInterval.duration() < 100) {
      targetTrack = CARRIAGE_CLEAN_POS;
    }
    if (movetoPosition(targetTrack, CARRIAGE_MAX_SPEED)) {
      // Nothing      
    }
    return;
  }
} // stateUpdate()


void Carriage::gotoNextTrack() {
  LOG_DEBUG("carriage.cpp", "[gotoNextTrack]");

  float pos = positionFilter;

  if (_shared.state == S_GOTO_TRACK) {
    pos = targetTrack;
  }

  int track = _scanner.trackCount - 1;
  if (track <= 0) {
    stopOrRepeat();
    return;
  }

  while ((pos - 2) <= _scanner.tracks[track]) { // 2mm offset to prevent repeating the same track
    track--;
    if (track <= 0) {
      stopOrRepeat();
      return;
    }
  }
  gotoTrack(_scanner.tracks[track]);
} // gotoNextTrack()


void Carriage::gotoPreviousTrack() {
  LOG_DEBUG("carriage.cpp", "[gotoPreviousTrack]");

  float pos = positionFilter;

  if (_shared.state == S_GOTO_TRACK) {
    pos = targetTrack;
  }

  int track = 0;
  while ((pos + CARRIAGE_BACKTRACK_OFFSET) >= _scanner.tracks[track]) {
    track++;
    if (track > (_scanner.trackCount - 1)) {
      gotoRecordStart();
      return;
    }
  }

  if (_scanner.tracks[track] > _scanner.recordStart) {
    gotoRecordStart();
  } else {
    gotoTrack(_scanner.tracks[track]);
  }
} // gotoPreviousTrack()


void Carriage::gotoTrack(float pos) {
  LOG_DEBUG("carriage.cpp", "[gotoTrack]");
  targetTrack = pos;
  // LOG_DEBUG("carriage.cpp", "[gotoTrack] To position " + String(targetTrack));
  Serial.println("To position " + String(targetTrack));
  _shared.setState(S_GOTO_TRACK);
} // gotoTrack()


void Carriage::gotoRecordStart() {
  LOG_DEBUG("carriage.cpp", "[gotoRecordStart]");
  gotoTrack(_scanner.recordStart);
} // gotoRecordStart()

bool Carriage::movetoPosition(float target, float spd) {
  _acceleration = 0;

  float togo = abs(target - position);
  int togoDirection = (target - position) > 0 ? 1 : -1;

  _distanceToStop = (_speed * _speed) / ( 2 * CARRIAGE_ACCELERATION );
  int _distanceToStopDirection = _speed > 0 ? 1 : -1;

  if (isApprox(togo, 0, 0.01) && _distanceToStop < 0.1) {
    _speed = 0;
    return true;
  }

  if (_distanceToStop >= togo) {
    _acceleration = -CARRIAGE_ACCELERATION * togoDirection;
  } else if (abs(_speed) < CARRIAGE_MAX_SPEED) {
    _acceleration = CARRIAGE_ACCELERATION * togoDirection;
  }

  position += _speed + (_acceleration / 2);
  _speed += _acceleration;
  _speed = limitFloat(_speed, -spd, spd);

  return false;
} // movetoPosition()


void Carriage::stopOrRepeat() {
  LOG_DEBUG("carriage.cpp", "[stopOrRepeat]");
  if (repeat) {
    gotoRecordStart();
  } else {
    _plateau.stop();
  }
} // stopOrRepeat()


void Carriage::pause() {
  LOG_DEBUG("carriage.cpp", "[pause]");
  if (_shared.state == S_PLAYING) {
    _shared.setState(S_PAUSE);
    targetTrack = position;
  } else if (_shared.state == S_PAUSE) {
    _shared.setState(S_PLAY_TILL_END);
  }
} // pause()


bool Carriage::decelerate() {
  int direction = _speed > 0 ? 1 : -1;

  if(abs(_speed) < CARRIAGE_ACCELERATION){
    _speed = 0;
    return true;
  }

  _acceleration = -CARRIAGE_ACCELERATION * direction;
  position += _speed + (_acceleration / 2);
  _speed += _acceleration;
  return false;
} // decelerate()


void Carriage::emergencyStop() {
  LOG_DEBUG("carriage.cpp", "[emergencyStop]");
  _speed = 0;
} // emergencyStop()


void Carriage::printGraphicData() {
  if (!_headerShown) {
    Serial.println("GRAPH_HEADER: DComp, ArmAngleRaw, ArmAngleCall");
    _headerShown = true;
  }
  Serial.print(_Dcomp, 5);
  Serial.print(", ");
  Serial.print(_arm.armAngleRaw);
  Serial.print(", ");
  Serial.print(_arm.armAngleCall, 2);
  Serial.println();
} // printGraphicData()


void Carriage::info() {
  int padR = 25;
  Serial.println(padRight("CARRIAGE_P", padR) + ": " + String(P, 5));
  Serial.println(padRight("CARRIAGE_I", padR) + ": " + String(I, 5));
  Serial.println(padRight("CARRIAGE_D", padR) + ": " + String(D, 5));
  Serial.println(padRight("CARRIAGE_POSITION", padR) + ": " + String(position));
  Serial.println(padRight("CARRIAGE_REAL_POS", padR) + ": " + String(realPosition));
  Serial.println();
} // info()
