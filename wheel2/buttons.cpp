#include "log.h"
#include "buttons.h"
#include "pins.h"
#include "helper.h"


Buttons::Buttons(Shared& shared, Amplifier& amplifier, Arm& arm, Bluetooth& bluetooth, Carriage& carriage, Orientation& orientation, Plateau& plateau, Scanner& scanner) :
  _shared(shared),
  _amplifier(amplifier),
  _arm(arm),
  _bluetooth(bluetooth),
  _carriage(carriage),
  _orientation(orientation),
  _plateau(plateau),
  _scanner(scanner),
  _interval(10000, TM_MICROS),
  rpmDisplayActionInterval(0, TM_MILLIS),
  volumeDisplayActionInterval(0, TM_MILLIS),
  ledBlinkInterval(0, TM_MILLIS),
  _allButtonsInterval(0, TM_MILLIS) {
} // Buttons()


void Buttons::init() {
  LOG_DEBUG("buttons.cpp", "[init]");
  // Nothing to do really ;)
} // init()


void Buttons::update() {
  if (_interval.tick()) {
    readData();

    for (int button = BUTTON_NEXT; button <= BUTTON_PREV; button++) {
      logic(button);
    }

    potVal = analogRead(DISPLAY_POTMETER_PIN);
    if (!isApprox(potVal, potValPrev, ARM_AMAX / 2)) {
      if (potVal > potValPrev) {
        potValPrev += ARM_AMAX;
      } else {
        potValPrev -= ARM_AMAX;
      }
    }

    belt +=  (float(potVal - potValPrev) * 80 ) / ARM_AMAX;
    potValPrev = potVal;
    beltFilter += (belt - beltFilter) / 3;

    if (!isApprox(beltFilter, beltFilterPrev, 1)) {
      beltDiff = beltFilter - beltFilterPrev;
      beltFilterPrev = beltFilter;

      if (millisSinceBoot() < 1000) { // belt action only after 1 sec.
        return;
      }

      if (_shared.state == S_NEEDLE_CLEAN && _arm.motorOn) {
        _arm.targetWeight += beltDiff * 0.0333;
        _arm.targetWeight = limitFloat(_arm.targetWeight, ARM_MIN_WEIGHT, ARM_MAX_WEIGHT);
      }
      if (!_orientation.isStanding) {
        beltDiff = -beltDiff; // flip
      }

      if (_shared.state == S_CALIBRATE) {
        _arm.force += beltDiff * 0.001;
        _arm.force = limitFloat(_arm.force, 0, 1);

      } else if(_shared.state == S_PAUSE) {
        _carriage.targetTrack -= beltDiff * 0.25;
        _carriage.targetTrack = limitFloat(_carriage.targetTrack, _scanner.tracks[0], _scanner.recordStart);

      } else {
        // to prevent volume popping up after button press while skipping
        if (_shared.state != S_SKIP_FORWARD && _shared.state != S_SKIP_REVERSE && _shared.state != S_GOTO_TRACK && _shared.state != S_PAUSE) { 
          volumeDisplayActionInterval.reset();
        }

        _amplifier.volume += round(beltDiff);
        _amplifier.volume = limitInt(_amplifier.volume, 0, 63);
      }
    }
  } // _interval.tick()
} // update()


void Buttons::readData() {
  gpio_put(DISPLAY_LATCH_PIN, 0);
  delayMicroseconds(1);
  gpio_put(DISPLAY_LATCH_PIN, 1);

  for (int button = 0; button < BUTTON_COUNT; button++) {
    delayMicroseconds(1);
    _buttonIn[button] = digitalRead(DISPLAY_OUT_PIN);

    gpio_put(DISPLAY_CLOCK_PIN, 1);
    delayMicroseconds(1);
    gpio_put(DISPLAY_CLOCK_PIN, 0);
  }  
} // readData()


void Buttons::logic(int button) {
  //--------------------------------------------- SHORT PRESS
  if (state[button] == BUTTON_RELEASE && _buttonIn[button] == BUTTON_PRESS) {
    state[button] = BUTTON_PRESS;

    _allButtonsInterval.reset();
    _buttonInterval[button] = millisSinceBoot();

    ledBlink();

    log(button, "PRESS");

    if (button == BUTTON_PLAY) {
      if (_shared.state == S_HOMING_BEFORE_PLAYING || _shared.state == S_GOTO_RECORD_START) {
        _shared.puristMode = true;
        Serial.println("PURIST MODE: ON");
      }

      if (_shared.state == S_HOME ) {
        _plateau.play();
        state[button] = BUTTON_LONG_PRESS; // To prevent stopping by long press
      }
    }

    if (_shared.state == S_NEEDLE_CLEAN || _shared.state == S_RECORD_CLEAN) { // Stop clean mode
      _plateau.stop();
    }
    return;
  }

  //--------------------------------------------- SHORT RELEASE
  if (state[button] == BUTTON_PRESS && _buttonIn[button] == BUTTON_RELEASE) {
    state[button] = BUTTON_RELEASE;
    _allButtonsInterval.reset();
    log(button, "RELEASE");

    if (isButtonNext(button)) {
      if (_shared.state == S_PLAYING || _shared.state == S_PAUSE || _shared.state == S_GOTO_TRACK) {
        _carriage.gotoNextTrack();
      }
    }

    if (isButtonPrev(button)) {
      if (_shared.state == S_PLAYING || _shared.state == S_PAUSE || _shared.state == S_GOTO_TRACK) {
        _carriage.gotoPreviousTrack();
      }
    }

    if (button == BUTTON_PLAY) {
      if (_shared.state == S_PAUSE || _shared.state == S_PLAYING) {
        _carriage.pause();
      }
    }

    //--------------------------------------------- RPM
    if (button == BUTTON_PREV && _shared.state == S_HOME) {
      if (_plateau.rpmMode == RPM_AUTO) {
        _plateau.rpmMode = RPM_33;
      } else if (_plateau.rpmMode == RPM_33) {
        _plateau.rpmMode = RPM_45;
      } else if (_plateau.rpmMode == RPM_45) {
        if (PLATEAU_ENABLE_RPM78) {
          _plateau.rpmMode = RPM_78;
        } else {
          _plateau.rpmMode = RPM_AUTO;
        }
      } else if (_plateau.rpmMode == RPM_78) {
        _plateau.rpmMode = RPM_AUTO;
      }
      _plateau.updateRpm();
      rpmDisplayActionInterval.reset();
    }

    //--------------------------------------------- BLUETOOTH
    if (button == BUTTON_NEXT && _shared.state == S_HOME) {
       LOG_DEBUG("buttons.cpp", "[logic] Bluetooth");
      _bluetooth.write("AT+DELVMLINK");
    }
    return;
  }

  //--------------------------------------------- LONG PRESS
  if (state[button] == BUTTON_PRESS && millisSinceBoot() - _buttonInterval[button] > BUTTON_LONG_CLICK) {
    state[button] = BUTTON_LONG_PRESS;
    _allButtonsInterval.reset();

    log(button, "LONG_PRESS");

    // >> FORWARD
    if ((_shared.state == S_PLAYING || _shared.state == S_PAUSE || _shared.state == S_GOTO_TRACK) && isButtonNext(button)) {
      _shared.setState(S_SKIP_FORWARD);
    }

    // << REVERSE
    if ((_shared.state == S_PLAYING || _shared.state == S_PAUSE || _shared.state == S_GOTO_TRACK) && isButtonPrev(button)) {
      _shared.setState(S_SKIP_REVERSE);
    }

    if (button == BUTTON_PLAY && _shared.state != S_HOME) {
      _plateau.stop();
    }

    //--------------------------------------------- BLUETOOTH RESET
    if (button == BUTTON_NEXT && _shared.state == S_HOME) {
      LOG_DEBUG("buttons.cpp", "[logic] Bluetooth reset");
      _bluetooth.write("AT+REST");
    }
    return;
  }

  //--------------------------------------------- LONG RELEASE
  if (state[button] == BUTTON_LONG_PRESS && _buttonIn[button] == BUTTON_RELEASE) {
    state[button] = BUTTON_RELEASE;
    _allButtonsInterval.reset();

    log(button, "LONG_PRESS RELEASE");

    if ((_shared.state == S_SKIP_FORWARD || _shared.state == S_SKIP_REVERSE) &&
      (button == BUTTON_NEXT || button == BUTTON_PREV)) { // Resume after forward/reverse
      _carriage.targetTrack = _carriage.position;
      _shared.setState(S_RESUME_AFTER_SKIP);
    }
    return;
  }

  //--------------------------------------------- SUPER LONG PRESS
  if (state[button] == BUTTON_LONG_PRESS && millisSinceBoot() - _buttonInterval[button] > BUTTON_SUPERLONG_CLICK) {
    state[button] = BUTTON_SUPERLONG_PRESS;
    _allButtonsInterval.reset();

    log(button, "SUPERLONG_PRESS");

    if (button == BUTTON_PLAY) {
      if (_shared.state == S_HOMING_BEFORE_PLAYING || _shared.state == S_GOTO_RECORD_START) { // Repeat
        _carriage.repeat = true;
      }
    }

    if (_shared.state == S_HOME && button == BUTTON_PREV) { // Clean mode
      _plateau.cleanMode();
      ledBlink();
    }
    return;
  }

  //--------------------------------------------- SUPER LONG RELEASE
  if (state[button] == BUTTON_SUPERLONG_PRESS && _buttonIn[button] == BUTTON_RELEASE) {
    state[button] = BUTTON_RELEASE;
    _allButtonsInterval.reset();

    log(button, "SUPERLONG_PRESS RELEASE");

    if ((_shared.state == S_SKIP_FORWARD || _shared.state == S_SKIP_REVERSE)  &&  
      (button == BUTTON_NEXT || button == BUTTON_PREV)) { // Resume after forward/reverse
      _carriage.targetTrack = _carriage.position;
      _shared.setState(S_RESUME_AFTER_SKIP);
    }
    return;
  }

  if (_buttonIn[button] == BUTTON_RELEASE) { // last check for button release to prevent long press loop
    state[button] = BUTTON_RELEASE;
    return;
  }
} // logic()


void Buttons::ledBlink() {
  ledBlinkInterval.reset();
} // ledBlink()


bool Buttons::isButtonNext(int button) {
  return button == buttonNextComp();
} // isButtonNext()


bool Buttons::isButtonPrev(int button) {
  return button == buttonPrevComp();
} // isButtonPrev()


int Buttons::buttonNextComp() {
  if (_orientation.isStanding) {
    return BUTTON_PREV;
  }
  return BUTTON_NEXT;
} // buttonNextComp()


int Buttons::buttonPrevComp() {
  if (_orientation.isStanding) {
    return BUTTON_NEXT;
  }
  return BUTTON_PREV;
} // buttonPrevComp()


void Buttons::log(int button, String action) {
    LOG_DEBUG("buttons.cpp", "[logic] " + getButton(button) + ": " + action);
  // Serial.println(getButton(button) + ": " + action);
} // log()


String Buttons::getButton(int button) {
  String strButton = "BUTTON_UNKNOWN";

  if        (button == BUTTON_PLAY) { strButton = "BUTTON_PLAY";
  } else if (button == BUTTON_NEXT) { strButton = "BUTTON_NEXT";
  } else if (button == BUTTON_PREV) { strButton = "BUTTON_PREV";
  }
  return strButton;
} // getButton()


void Buttons::info() {
  int padR = 25;
  Serial.print(padRight("BUTTONS", padR) + ": ");
  for (int button = 0; button < BUTTON_COUNT; button++) {
    Serial.print(String(_buttonIn[button]) + " ");
  }
  Serial.println();
} // info()
