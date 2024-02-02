#include "log.h"
#include "speedcomp.h"
#include "carriage.h"
#include "pins.h"
#include "helper.h"
#include "plateau.h"


SpeedComp::SpeedComp(Shared& shared, Arm& arm) :
  _shared(shared),
  _arm(arm) {
} // SpeedComp()


void SpeedComp::init(Carriage* carriage, Plateau* plateau) { // to prevent circular reference
  LOG_DEBUG("speedcomp.cpp", "[init]");
  _carriage = carriage;
  _plateau = plateau;
  float radialCounter;
  clearSamples();
  clearCompSamples();

  for (int i = 0; i < pulsesPerRev; i++) {
    radialCounter = (i * SPEEDCOMP_TAU) / pulsesPerRev;
    _sinus[i] = sin(radialCounter);
    _cosin[i] = cos(radialCounter);
  }

  createUnbalanceFilterCurve();
} // init()


void SpeedComp::update() {
  if ((microsSinceBoot() - _speedInterval) > SPEEDCOMP_SAMPLES_MAX ) {
    if (_glitchCounter > 3) {
      shiftSamples(SPEEDCOMP_SAMPLES_MAX * _direction);
      speedRaw = 0;
      speed += (speedRaw - speed) / 10;
      speedLowPass += (speed - speedLowPass) / 100;
    } else {
      _glitchCounter++;
    }
  } else {
    _glitchCounter = 0;
  }
} // update()


void SpeedComp::stroboInterrupt() {
  _time = microsSinceBoot();
  _processTime = microsSinceBoot();

  //------------------------------------------------------------ DIRECTION
  _direction = 1;
  _sens = (gpio_get(PLATEAU_A_PIN) << 1 ) | gpio_get(PLATEAU_B_PIN);
  if (_sens == 0b00 && _sensPrev == 0b01 ||
      _sens == 0b01 && _sensPrev == 0b11 ||
      _sens == 0b11 && _sensPrev == 0b10 ||
      _sens == 0b10 && _sensPrev == 0b00) {
    _direction = -1;
  }
  _sensPrev = _sens;

  // Serial.println(String(_direction == 1 ? "Anti-Clockwise" : "Clockwise"));
  if (_directionPrev != _direction) {
    // LOG_DEBUG("speedcomp.cpp", "[stroboInterrupt] Direction changed: " + String(_direction == 1 ? "CW -> ACW" : "ACW -> CW"));
    clearUnbalanceCompSamples();
  }
  _directionPrev = _direction;


  //------------------------------------------------------------ SPEED
  _interval = _time - _speedInterval;
  _speedInterval = _time;

  if (_interval > SPEEDCOMP_SAMPLES_MAX) {
    _interval = SPEEDCOMP_SAMPLES_MAX;
  }

  _counterRaw += _direction;
  _counterSinceReset += _direction;
  counter = roundTrip(counter + _direction, pulsesPerRev);

  shiftSamples(_interval);
  getSpeed();
  speed += (speedRaw - speed) / 10;
  _processInterval = microsSinceBoot() - _processTime;

  if (counter == 0) { // one rotation
    if (_clearCompSamplesQueue) { // T =0, Comp reset
      _clearCompSamplesQueue = false;
      _counterSinceReset = 0;
      clearUnbalanceCompSamples();
    }
  }


  //------------------------------------------------------------ OFF CENTER COMPENSATION
  _carriagePosMiddlePre -= _carriageOffCenterWave[counter];
  _carriageOffCenterWave[counter] = _carriage->realPosition;
  _carriagePosMiddlePre += _carriageOffCenterWave[counter];
  carriagePosMiddle = _carriagePosMiddlePre / pulsesPerRev;

  trackSpacing = _carriagePosCenterHist[counter] - carriagePosMiddle;
  _carriagePosCenterHist[counter] = carriagePosMiddle;

  // if (trackSpacing > 0.01 || !_arm.isNeedleDownFor(2000)) {
  //   _carriage->movedForwardInterval.reset();
  // } else {
  //   // Nothing
  // }

  float carriagePosOffCenter = _carriage->realPosition - carriagePosMiddle;

  if (_arm.isNeedleDownFor(1000) && _shared.state == S_PLAYING) { // needle has to be down while playing before calculation
    _carriageSin -= _carriageSinValues[counter];
    _carriageSinValues[counter] = _sinus[counter] * carriagePosOffCenter;
    _carriageSin += _carriageSinValues[counter];
    
    _carriageCos -= _carriageCosValues[counter];
    _carriageCosValues[counter] = _cosin[counter] * carriagePosOffCenter;
    _carriageCos += _carriageCosValues[counter];

    _carriageSinFilt += (_carriageSin - _carriageSinFilt) / 2000;
    _carriageCosFilt += (_carriageCos - _carriageCosFilt) / 2000;
  }

  carriageFourier  = ( ( (_sinus[counter] * _carriageSin) + ( _cosin[counter] * _carriageCos) ) / pulsesPerRev ) * 2;
  carriageFourierFilter  = ( ( ( _sinus[counter] * _carriageSinFilt )  +  ( _cosin[counter] * _carriageCosFilt ) ) / pulsesPerRev) * 2;
  

  //------------------------------------------------------------ too big break-out ERROR
  float sinBuff = _carriageSinFilt / pulsesPerRev;
  float cosBuff = _carriageCosFilt / pulsesPerRev;
  // an off-center of 6mm (3mm radius) triggers error
  if ((sinBuff * sinBuff + cosBuff * cosBuff) > (3 * 3)) {
    _shared.setError(E_TO_MUCH_TRAVEL);
    clearCompSamples();
    _plateau->stop();
  }


  //------------------------------------------------------------ COMP SPEEDS
  // phase shift: 8 of 16 samples avg filter, and 9 from found filter
  int leadCounter = roundTrip(counter - (8+9), pulsesPerRev);
  float offCenterSpeedComp = ( ( (_sinus[leadCounter] * _carriageSinFilt) + (_cosin[leadCounter] * _carriageCosFilt) ) / pulsesPerRev) * 2;

  _centerComp = ((carriagePosMiddle - offCenterSpeedComp) / carriagePosMiddle);
  centerCompTargetRpm = _plateau->targetRpm * _centerComp;
  
  if (recordOffCenterComp) {
    speedCenterComp = speed / _centerComp;
  } else {
    speedCenterComp = speed;
  }


  //------------------------------------------------------------ WOW FLUTTER MEASUREMENTS
  speedLowPass += (speedCenterComp - speedLowPass) / 100;
  speedHighPass = speedCenterComp - speedLowPass;

  lowpassRect = abs(speedLowPass - _plateau->targetRpm);
  if (lowpassRect > wow) {
    wow = lowpassRect;
  } else {
    wow += (lowpassRect - wow) / 1000;
  }

  if (wow < 0.1 && _wowFirstLow) { // _wowFirstLow == true
    _wowFirstLow = false;
    // LOG_DEBUG("speedcomp.cpp", "[stroboInterrupt] Runs synchrone again after " + String(_counterSinceReset / float(pulsesPerRev)) + " turns");
    // LOG_DEBUG("speedcomp.cpp", "[stroboInterrupt] Unbalance Phase       : " + String(unbalancePhase));
    // LOG_DEBUG("speedcomp.cpp", "[stroboInterrupt] Unbalance Comp Weight : " + String(unbalanceCompWeight));
    // LOG_DEBUG("speedcomp.cpp", "[stroboInterrupt] Unbalance Filter Width: " + String(unbalanceFilterWidth));
    _counterSinceReset = 0;
  }

  if (wow > 0.3 && !_wowFirstLow) { // _wowFirstLow == false
    _wowFirstLow = true;
  }


  //------------------------------------------------------------ UNBALANCE COMPENSATION
  if (unbalanceCompOn                      // all prereqs when compensation should be off
      && _plateau->motorOn 
      && _plateau->turnInterval.duration() > 1000 // should be on for 1 sec.
      && _plateau->atSpeed                        // and speeded up
      && isApprox(speed, _plateau->targetRpm, 10) // not more than 10rpm from target rpm
      && ((_arm.isNeedleDownFor(2000) && _shared.state == S_PLAYING) ||
      _shared.state == S_HOMING_BEFORE_PLAYING ||    
      _shared.state == S_GOTO_RECORD_START)) { 
    
    int speedError = (speedCenterComp - _plateau->targetRpm) * 1000.0;
    int value;
    for (int i = 0; i < _unbalanceFilterCurveWidth; i++) {
      value = _unbalanceFilterCurve[i] * speedError;
      _unbalansComp[(counter + 1 + i) % pulsesPerRev] += value;
      _unbalansComp[(counter + pulsesPerRev - i) % pulsesPerRev] += value;
    }
    // digitalWrite(LED_PIN, 1); // turn led on
  } else {
    // digitalWrite(LED_PIN, 0); // turn led off
  }

  if (unbalanceCompOn) {
    unbalanceComp = - (_unbalansComp[roundTrip(counter + unbalancePhase, pulsesPerRev)] / (100000000.0)) * unbalanceCompWeight;
  } else {
    unbalanceComp = 0;
  }

  if (graphicData) {
    printGraphicData();
  } else {
    _headerShown = false;
  }
} // stroboInterrupt()


void SpeedComp::clearCompSamplesOnT0() {
  LOG_DEBUG("speedcomp.cpp", "[clearCompSamplesOnT0]");
  _clearCompSamplesQueue = true;
} // clearCompSamplesOnT0()


void SpeedComp::clearSamples() {
  LOG_DEBUG("speedcomp.cpp", "[clearSamples]");
  for (int i = 0; i < samples; i++) {
    _samplesArr[i] = SPEEDCOMP_SAMPLES_MAX;
  }
} // clearSamples()


void SpeedComp::clearCompSamples() {
  LOG_DEBUG("speedcomp.cpp", "[clearCompSamples]");
  clearUnbalanceCompSamples();
  clearCenterCompSamples();
} // clearCompSamples()


void SpeedComp::clearUnbalanceCompSamples() {
  // LOG_DEBUG("speedcomp.cpp", "[clearUnbalanceCompSamples]");
  for (int i = 0; i < pulsesPerRev; i++) {
    _unbalansComp[i] = 0;
  }
} // clearUnbalanceCompSamples()


void SpeedComp::clearCenterCompSamples() {
  LOG_DEBUG("speedcomp.cpp", "[clearCenterCompSamples]");
  float pos = _carriage->realPosition;

  for (int i = 0; i < pulsesPerRev; i++) {
    _carriageSinValues[i] = 0;
    _carriageCosValues[i] = 0;
    _carriageOffCenterWave[i] = pos;
    _carriagePosCenterHist[i] = pos;
  }

  _carriageSinFilt = 0;
  _carriageCosFilt = 0;
  _carriageSin = 0;
  _carriageCos = 0;

  _carriagePosMiddlePre = pos * pulsesPerRev;
  carriagePosMiddle = pos;
} // clearCenterCompSamples()


void SpeedComp::createUnbalanceFilterCurve(){
  LOG_DEBUG("speedcomp.cpp", "[createUnbalanceFilterCurve]");

  float total = 0;
  _unbalanceFilterCurveWidth = pulsesPerRev / 2;

  for (int i = 0; i < pulsesPerRev / 2; i++) {
    float j = float(i) / pulsesPerRev;
    float value = exp(-unbalanceFilterWidth * (j*j));

    if (value > 0.01) {
      _unbalanceFilterCurve[i] = value * 1000;
    } else {
      if ( _unbalanceFilterCurveWidth > i ) {
        _unbalanceFilterCurveWidth = i;
      }
    }
  }
} // createUnbalanceFilterCurve()


float SpeedComp::getSpeed() {
  _average = averageInterval();
  speedRaw = currentSpeed(_average) * _direction;
  return speedRaw; // don't compensate
} // getSpeed()


float SpeedComp::averageInterval() {
  int total = 0;

  for (byte i = 0; i < SPEEDCOMP_SAMPLES; i++) {
    total += _samplesArr[i];
  }
  return total / float(SPEEDCOMP_SAMPLES);
} // averageInterval()


float SpeedComp::currentSpeed(float inter) { // Calculate rpm
  float value = ((1000000.0 * 60) / inter) / pulsesPerRev; // return total
  return limitFloat(value, -300, 300);
} // currentSpeed()


void SpeedComp::shiftSamples(int sample) {
  _samplesArr[_sampleCounter++ % samples] = sample;
} // shiftSamples()


void SpeedComp::printGraphicData() {
  if (!_headerShown) {
    Serial.println("GRAPH_HEADER: SpeedRaw, Speed, CarriageFourier");
    _headerShown = true;
  }
  Serial.print(speedRaw, 3);
  Serial.print(",");
  Serial.print(speed, 3);
  Serial.print(",");
  Serial.print(carriageFourier, 3);
  Serial.println();
}


void SpeedComp::info() {
  int padR = 25;
  // Serial.println(padRight("STROBO_SAMPLES", padR) +            ": " + String(samples));
  // Serial.println(padRight("STROBO_PULSES_PER_REV", padR) +     ": " + String(pulsesPerRev));
  Serial.println(padRight("STROBO_UNBAL_PHASE", padR) +        ": " + String(unbalancePhase));
  Serial.println(padRight("STROBO_UNBAL_COMP_WEIGHT", padR) +  ": " + String(unbalanceCompWeight));
  Serial.println(padRight("STROBO_UNBAL_FILT_WIDTH", padR) +   ": " + String(unbalanceFilterWidth));
  Serial.println(padRight("STROBO_UNBAL_FILT_CURVE_W", padR) + ": " + String(_unbalanceFilterCurveWidth));

  Serial.println();
} // info()
