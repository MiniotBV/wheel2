#include "log.h"
#include "scanner.h"
#include "carriage.h"
#include "pins.h"
#include "pwm.h"
#include "helper.h"


Scanner::Scanner(Shared& shared, Plateau& plateau) :
  _shared(shared),
  _plateau(plateau),
  _interval(10000, TM_MICROS) {
} // Scanner()


void Scanner::init(Carriage* carriage) { // to prevent circular reference
  LOG_DEBUG("scanner.cpp", "[init]");
  _carriage = carriage;
  setPwm(SCANNER_LED_PIN);
  setLedMilliAmp(0); // 10mA
} // init()


void Scanner::func() {
  if (_interval.tick()) {
    if (_shared.state == S_HOME) { // if carriage @home, stop scanner
      clearTracks();
      scanLedOff();
      return;
    }

    _rawPrev = _raw;
    _raw = analogRead(SCANNER_PIN);
    _rawDiff = _raw - _rawPrev;
    _absDiff = abs(_rawDiff);
    _value = (_absDiff - _sensorTarget);

    recordDetection();

    if (_cut) {
      scanLedOff();

      float _ledCurrentP = (_sensorTarget - _absDiff) * _currentP;
      current += limitFloat(_ledCurrentP, -1, 1);
      current = limitFloat(current, 5, 30);
      _diff = _value - _valuePrev;
      _valuePrev = _value;

      scanForTracks();
    } else {
      setLedMilliAmp(current);
    }
    _cut = !_cut; // toggle led

    if (graphicData) {
      printGraphicData();
    } else {
      _headerShown = false;
    }
  } // _interval.tick()
} // func()


void Scanner::check() {
  LOG_DEBUG("scanner.cpp", "[check]");

  _bufferLength = _bufferCounter;
  _bufferCounter = 0;

  //---------------------------------------- find biggest peak
  float biggestPeak = 0;
  for (int i = 0; i < _bufferLength; i++) {
    if (_buffer[i][1] > biggestPeak) {
      biggestPeak = _buffer[i][1];
    }
  }
  LOG_DEBUG("scanner.cpp", "[check] Biggest peak: " + String(biggestPeak));

  //---------------------------------------- find all tracks
  clearTracks();

  float newThreshold = biggestPeak / 3;

  for (int i = 0; i < _bufferLength; i++) {
    float value = _buffer[i][1];
    
    if (value < (newThreshold / 2) && _trackBelowThreshold) {
      _trackBelowThreshold = false;
    }
    if (value > newThreshold && !_trackBelowThreshold) {
      _trackBelowThreshold = true;
      newTrack(_buffer[i][0]);
    }
  }
} // check()


void Scanner::setTracksAs7inch() {
  LOG_DEBUG("scanner.cpp", "[setTracksAs7inch]");
  trackCount = 1;
  tracks[1] = CARRIAGE_7INCH_START;

  if (tracks[0] > (CARRIAGE_7INCH_START - 12)) {
    tracks[0] = 55;
    // LOG_NOTICE("scanner.cpp", "[setTracksAs7inch] Adjusted record-end");
    Serial.println("Adjusted record-end");
  }
} // setTrackAs7inch()


void Scanner::newTrack(float pos) {
  // LOG_DEBUG("scanner.cpp", "[newTrack]");

  if (trackCount == 0) {
    LOG_DEBUG("scanner.cpp", "[newTrack] End record: " + String(pos));
  } else {
    float distance = pos - tracks[trackCount - 1];
    if (distance < 2) {
      return;
    }
    LOG_DEBUG("scanner.cpp", "[newTrack] Track at pos: " + String(pos));
  }

  tracks[trackCount] = pos;
  trackCount++;
} // newTrack()


void Scanner::recordDetection() {
  // LOG_DEBUG("scanner.cpp", "[recordDetection]");

  // when enough amplitude, a record is detected
  recordPresent = _absDiff > SCANNER_DETECTION_THRESHOLD;
  _recordPresentFiltered += (recordPresent - _recordPresentFiltered) / 10;

  // still a record present?
  if (!isRecordPresent() && (_shared.state == S_PLAYING || _shared.state == S_PAUSE)) {
    // LOG_NOTICE("scanner.cpp", "[recordDetection] Record removed?");
    Serial.println("Record removed?");
    _plateau.stop();
    return;
  }
} // recordDetection()


bool Scanner::isRecordPresent() {
  return _recordPresentFiltered > 0.5;
} // isRecordPresent()


void Scanner::scanForTracks() {
  // LOG_DEBUG("scanner.cpp", "[scanForTracks]");

  if (_carriage->sensorPosition < (CARRIAGE_RECORD_END + 2) || _shared.state != S_GOTO_RECORD_START) {
    _bufferCounter = 0;
    return;
  }

  float value = -_diff;

  _buffer[_bufferCounter][0] = _carriage->sensorPosition; // save for after-check
  _buffer[_bufferCounter][1] = value;
  _bufferCounter++;

  if (value < (_trackThreshold / 2) && _trackBelowThreshold) {
    _trackBelowThreshold = false;
  }
  if (value > _trackThreshold && !_trackBelowThreshold) {
    _trackBelowThreshold = true;
    newTrack(_carriage->sensorPosition);
  }
} // scanForTracks()


void Scanner::clearTracks() {
  // LOG_DEBUG("scanner.cpp", "[clearTracks]");
  trackCount = 0;
  tracks[trackCount] = 1;
} // clearTracks()


void Scanner::scanLedOff() {
  // LOG_DEBUG("scanner.cpp", "[scanLedOff]");
  pwmWrite(SCANNER_LED_PIN, 0); // 100ohm + 1volt led drop
} // scanLedOff()


void Scanner::setLedMilliAmp(float amp) {
  // LOG_DEBUG("scanner.cpp", "[setLedMilliAmp]");
  amp /= 1000.0;
  pwmWrite(SCANNER_LED_PIN, volt2pwm(1 + (100 * amp))); // 100ohm + 1volt led drop
} // setLedMilliAmp()


int Scanner::volt2pwm(float volt) {
  return (volt * PWM_PMAX) / 3.3;
} // volt2pwm()


void Scanner::printGraphicData() {
  if (!_headerShown) {
    Serial.println("GRAPH_HEADER: Value, AbsDiff, Diff, Current");
    _headerShown = true;
  }
  Serial.print(_value);
  Serial.print(", ");
  Serial.print(_absDiff);
  Serial.print(", ");
  Serial.print(_diff);
  Serial.print(", ");
  Serial.print(current * 100);
  Serial.println();
} // printGraphicData()


void Scanner::info() {
  int padR = 25;
  Serial.println(padRight("SCANNER_TOTAL_TRACKS", padR) +  ": " + String(trackCount));
  if (trackCount > 0 ) {
    Serial.println(padRight("SCANNER_TRACK_1", padR) +  ": " + String(recordStart));
    for (int t = trackCount - 1; t > 0; t--) {
      Serial.println(padRight("SCANNER_TRACK_" + String(trackCount - t + 1), padR) +  ": " + String(tracks[t]));
    }
    Serial.println(padRight("SCANNER_RECORD_END", padR) +  ": " + String(tracks[0]));
  }
  Serial.println();
} // info()
