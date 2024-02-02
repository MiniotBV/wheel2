#include "log.h"
#include "serialcomm.h"
#include "pins.h"
#include "helper.h"


SerialComm::SerialComm(Shared& shared, Amplifier& amplifier, Arm& arm, Bluetooth& bluetooth, Buttons& buttons, Carriage& carriage,
      Orientation& orientation, Plateau& plateau, Scanner& scanner, SpeedComp& speedcomp, Storage& storage) :
      _shared(shared),
      _amplifier(amplifier),
      _arm(arm),
      _bluetooth(bluetooth),
      _buttons(buttons),
      _carriage(carriage),
      _orientation(orientation),
      _plateau(plateau),
      _scanner(scanner),
      _speedcomp(speedcomp),
      _storage(storage),
      _interval(10000, TM_MICROS) {
} // SerialComm()


void SerialComm::init() {
    // Start serial output
  Serial.begin(SERIAL_BAUDRATE);

  // Wait until the serial stream is open
  delay(1000); // Needed, otherwise you miss the debug log's in all the init-void's
  // while (!Serial); // <- don't use this, as it waits for ages for a serial connection before is will startup

  if (Serial) {
    LOG_INFO("serialcomm.cpp", "[init] Serial port has been openend at " + String(SERIAL_BAUDRATE) + " bps (baud)");
  } else {
    LOG_CRITICAL("serialcomm.cpp", "[init] Serial port has not been opened!");
  }
} // init()


void SerialComm::func() {
  if (_interval.tick()) {

    if (_graphicData) {
      printGraphicData();
    } else {
      _headerShown = false;
    }

    while(Serial.available() > 0) {
      char letter = Serial.read();
      
      if ((letter == '\n' || letter == '\r') && _line != "") {
        _lineRaw = _line;
        _line.trim();
        _line.toLowerCase();

        if (_line.startsWith("l")) { // 'L' is last command; add previous command to _line
          _line.replace("l", _lastCommand);
        }

        checkReceivedLine(_line);
        _line = "";
      } else {
        _line += letter;
      }
    }
  }
} // func()


void SerialComm::checkReceivedLine(String line, eCheckMode mode) {
  LOG_DEBUG("serialcomm.cpp", "[checkReceivedLine]");
  println(mode);
  if (checkLineCommand( "RST",    "Reboot",                     mode)) { rp2040.reboot();                     return; }
  // if (checkLineCommand( "BOOT",   "Reboot to USB bootloader",   mode)) { rp2040.rebootToBootloader();         return; }

  if (checkLineCommand( "AT+",    "Bluetooth command",          mode)) { _bluetooth.write(_lineRaw);          return; }
  if (checkLineBool(    "BT",     "Bluetoot uart",              mode,  _bluetooth.debug)) {                   return; }

  if (checkLineBool(    "G",      "Graphics",                   mode, _graphicData)) {                        return; }
  if (checkLineBool(    "PLG",    "RecordScanner graphics",     mode, _scanner.graphicData)) {                return; }
  if (checkLineBool(    "KG",     "Carriage graphics",          mode, _carriage.graphicData)) {               return; }
  if (checkLineBool(    "SG",     "Strobo graphics",            mode, _speedcomp.graphicData)) {              return; }
  if (checkLineBool(    "OG",     "Orientation graphics",       mode, _orientation.graphicData)) {            return; }

  //-------------------------------------------------- STATE --------------------------------------------------
  println(mode);
  if (checkLineCommand( ">>",     "Next track",                 mode)) { _carriage.gotoNextTrack();           return; }
  if (checkLineCommand( "<<",     "Previous track",             mode)) { _carriage.gotoPreviousTrack();       return; }
  if (checkLineCommand( "HOK",    "Home",                       mode)) { _shared.setState(S_HOME);            return; }
  if (checkLineCommand( "STOP",   "Stop",                       mode)) { _plateau.stop();                     return; }
  if (checkLineCommand( "SPEEL",  "Play",                       mode)) { _plateau.play();                     return; }
  if (checkLineCommand( "PAUZE",  "Pause",                      mode)) { _carriage.pause();                   return; }
  if (checkLineCommand( "NAALD",  "Clean needle",               mode)) { _shared.setState(S_NEEDLE_CLEAN);    return; }
  if (checkLineCommand( "CAL",    "Calibrate",                  mode)) { _shared.setState(S_CALIBRATE);       return; }
  if (checkLineBool(    "REP",    "Repeat",                     mode, _carriage.repeat)) {                    return; }

  //-------------------------------------------------- ARM --------------------------------------------------
  println(mode);
  if (checkLineCommand( "NE",     "Needle down",                mode)) { _arm.putNeedleInGrove();             return; }
  if (checkLineCommand( "NA",     "Needle up",                  mode)) { _arm.dockNeedle();                   return; }
  if (checkLineFloat(   "ATG",    "Arm targetweight",           mode, _arm.targetWeight)) { _storage.saveRequired  = true; return; }
  if (checkLineFloat(   "AG",     "Arm weight",                 mode, _arm.weight)) {                         return; }
  if (checkLineCommand( "AKHOK",  "Arm force Docked calibrate", mode)) { _arm.justDockedWeight = _arm.weight; Serial.println(padRight("AKHOK", 8) + " " + padRight("Arm force Docked calibrate", 26) + " SET: " + String(_arm.justDockedWeight, 5)); _storage.saveRequired = true; return; }
  if (checkLineCommand( "AKL",    "Arm force 500mg calibrate",  mode)) { _arm.forceLow = _arm.force;          Serial.println(padRight("AKL", 8)   + " " + padRight("Arm force 500mg calibrate", 26)  + " SET: " + String(_arm.forceLow, 5));         _storage.saveRequired = true; return; }
  if (checkLineCommand( "AKH",    "Arm force 4000mg calibrate", mode)) { _arm.forceHigh = _arm.force;         Serial.println(padRight("AKH", 8)   + " " + padRight("Arm force 4000mg calibrate", 26) + " SET: " + String(_arm.forceHigh, 5));        _storage.saveRequired = true; return; }
  if (checkLineFloat(   "AK",     "Arm force",                  mode, _arm.force)) { _arm.force = limitFloat(_arm.force, 0, 1); return;}
 
  //-------------------------------------------------- CARRIAGE --------------------------------------------------
  println(mode);
  if (checkLineFloat(   "KP",     "Carriage P",                 mode, _carriage.P)) {                         return; }
  if (checkLineFloat(   "KI",     "Carriage I",                 mode, _carriage.I)) {                         return; }
  if (checkLineFloat(   "KD",     "Carriage D",                 mode, _carriage.D)) {                         return; }
  if (checkLineFloat(   "TNP",    "Target track",               mode, _carriage.targetTrack)) { _carriage.targetTrack = limitFloat(_carriage.targetTrack, CARRIAGE_HOME, CARRIAGE_12INCH_START); return; }

  //-------------------------------------------------- PLATEAU --------------------------------------------------
  println(mode);
  if (checkLineFloat(   "PP",     "Plateau P",                  mode, _plateau.P)) {                          return; }
  if (checkLineFloat(   "PI",     "Plateau I",                  mode, _plateau.I)) {                          return; }
  if (checkLineFloat(   "PD",     "Plateau D",                  mode, _plateau.D)) {                          return; }

  if (checkLineFloat(   "TR",     "Target RPM",                 mode, _plateau.targetRpm)) { _plateau.turnInterval.reset(); return; }

  if (checkLineCommand( "PA",     "Plateau start",              mode)) { _plateau.motorStart();               return; }
  if (checkLineCommand( "PS",     "Plateau stop",               mode)) { _plateau.motorStop();                return; }
  if (checkLineBool(    "PL",     "Plateau logica",             mode, _plateau.logic)) {                      return; }
  if (checkLineBool(    "PC",     "Unbalance compensation",     mode, _plateau.unbalanceCompensation)) {      return; }

  //-------------------------------------------------- STROBO --------------------------------------------------
  println(mode);
  // if (checkLineInt(     "SSN",    "Strobo samples",             mode, _speedcomp.samples)) {                  return; }
  if (checkLineBool(    "SOC",    "Strobo unbalance Comp. On",  mode, _speedcomp.unbalanceCompOn)) {          return; }
  if (checkLineBool(    "SKC",    "Strobo OffCenter Comp.",     mode, _speedcomp.recordOffCenterComp)) {      return; }
  if (checkLineBool(    "KC",     "Carriage OffCenter Comp.",   mode, _carriage.offCenterCompensation)) {     return; }

  if (checkLineFloat(   "SOG",    "Strobo unbal. Comp. Weight", mode, _speedcomp.unbalanceCompWeight)) {      return; }
  if (checkLineFloat(   "SOFB",   "Strobo unbal. Filter Width", mode, _speedcomp.unbalanceFilterWidth)) {_speedcomp.createUnbalanceFilterCurve(); return; }
  if (checkLineInt(     "SOF",    "Strobo unbal. Phase",        mode, _speedcomp.unbalancePhase)) {           return; }

  if (checkLineCommand( "SCZ",    "Strobo clearCompSamples On T0", mode)) { _speedcomp.clearCompSamplesOnT0(); return; }
  if (checkLineCommand( "SCC",    "Strobo clearCompSamples",    mode)) { _speedcomp.clearCompSamples();       return; }

  //-------------------------------------------------- STORAGE --------------------------------------------------
  println(mode);
  if (checkLineFloat(   "EV",     "eepromVersie",               mode, _storage.eepromVersion)) {              return; }
  if (checkLineCommand( "EO",     "Save EEPROM",                mode)) { _storage.write();                    return; }
  if (checkLineCommand( "EL",     "Read EEPROM",                mode)) { _storage.read();                     return; }
  if (checkLineCommand( "OC",     "Orientation calibrate",      mode)) { _orientation.calibrate(); _storage.saveRequired  = true; return; }
  if (checkLineFloat(   "TO",     "Track offset",               mode, _carriage.trackOffset)) { _storage.saveRequired  = true; return; }
  if (checkLineCommand( "AHCal",  "Calibrate arm angle",        mode)) { _arm.calibrateAngle(); _storage.saveRequired = true; return; }

  //-------------------------------------------------- CARRIAGE SENSORS --------------------------------------------------
  println(mode);
  // if(checkLineFloat(    "PLS",    "Scanner current",            mode, _scanner.current)) {                    return; }
  if (checkLineInt(     "VOLUME", "Volume w/o override",        mode, _amplifier.volume)) { _amplifier.volumeOverRide = false; return; }
  if (checkLineInt(     "VOL",    "Volume",                     mode, _amplifier.volume)) { _amplifier.volumeOverRide = true; return; }
  if (checkLineCommand( "AHCent", "Center Arm Angle",           mode)) { _arm.centerArmAngle();               return; }

  //-------------------------------------------------- HELP --------------------------------------------------
  println(mode);
  if (checkLineCommand( "C?",     "Show commands",              mode)) { checkReceivedLine(line, CM_COMMAND); return; }
  if (checkLineCommand( "CW",     "Show values",                mode)) { checkReceivedLine(line, CM_VALUE);   return; }
  if (checkLineCommand( "?",      "Report",                     mode)) { report();                            return; }
  if (checkLineCommand( "INFO",   "Info",                       mode)) { info();                              return; }

  if (mode == CM_NONE) {
    line.toUpperCase();
    Serial.println("Wrong command received: \"" + line + "\"");
  }
} // checkReceivedLine()


bool SerialComm::checkLineCommand(String command, String description, eCheckMode mode) {
  if (mode == CM_VALUE) {
    return false;
  }
  if (!checkLine(command, description, mode)) {
    return false;
  }
  Serial.println();
  return true;
} // checkLineCommand()


bool SerialComm::checkLine(String command, String description, eCheckMode mode) {
  if (mode == CM_COMMAND) {
    printCommando(command, description);
    Serial.println();
    return false;
  }
  command.toLowerCase();
  if (!_line.startsWith(command)) {
    return false;
  }
  _lastCommand = command;
  _line.replace(command, "");
  _line.trim();
  printCommando(command, description);
  return true;
} // checkLine()


bool SerialComm::checkLineInt(String command, String description, eCheckMode mode, int& value) {
  if (mode == CM_VALUE) {
    printValue(command, description, String(value));
    return false;
  }
  if (!checkLine(command, description, mode)) {
    return false;
  }
  if (_line.indexOf('?') == -1 && _line.length() != 0) { // If no '?' in command
    value = _line.toInt();
    Serial.print("SET: ");
  } else {
    Serial.print("GET: ");
  }
  Serial.println(String(value));
  return true;
} // checkLineInt()


bool SerialComm::checkLineFloat(String command, String description, eCheckMode mode, float& value) {
  if (mode == CM_VALUE) {
    printValue(command, description, String(value, 5));
    return false;
  }
  if (!checkLine(command, description, mode)) {
    return false;
  }
  if (_line.indexOf('?') == -1 && _line.length() != 0) { // If no '?' in command
    value = _line.toFloat();
    Serial.print("SET: ");
  } else {
    Serial.print("GET: ");
  }
  Serial.println(String(value, 5));
  return true;
} // checkLineFloat()


bool SerialComm::checkLineBool(String command, String description, eCheckMode mode, bool& value) {
  if (mode == CM_VALUE) {
    printValue(command, description, String(value));
    return false;
  }
  if (!checkLine(command, description, mode)) {
    return false;
  }
  if (_line.indexOf('?') != -1) { // If there is a '?' in command
    Serial.print("GET: ");
  } else if (isDigit(_line.charAt(0))) {
    value = _line.toInt();
    Serial.print("SET: ");
  } else {
    value = !value;
    Serial.print("TOGGLED: ");
  }   
  Serial.println(String(value));
  return true;
} // checkLineBool()


void SerialComm::println(eCheckMode mode) {
  if (mode != CM_NONE) {
    Serial.println();
  }
} // println()


void SerialComm::printCommando(String command, String description) {
  command.toUpperCase();
  Serial.print(padRight(command, 8) + " " + padRight(description, 26) + " ");
} // printCommando()


void SerialComm::printValue(String command, String description, String value) {
  command.toUpperCase();
  Serial.println(padRight(command, 8) + " " + padRight(description, 26) +  " " + value);
} // printValue()


void SerialComm::printGraphicData() {
  if (!_headerShown) {
    Serial.println("GRAPH_HEADER: SpeedRaw-TRPM, Speed-CTRPM, PPR, CTPRM-TRPM, UnbalanceComp, ArmAngleCall, RealPosition, Trackspacing, ArmWeight");
    _headerShown = true;
  }
  Serial.print(_speedcomp.speedRaw - _plateau.targetRpm, 3);
  Serial.print(", ");
  Serial.print(_speedcomp.speed - _speedcomp.centerCompTargetRpm, 3);

  // Serial.print(", ");
  // Serial.print(_speedcomp.speed, 3);
  Serial.print(", ");
  Serial.print((float)_speedcomp.rotationPosition / _speedcomp.pulsesPerRev, 3);

  // Serial.print(", ");
  // Serial.print(_speedcomp._unbalanceFilterCurve[_speedcomp.rotationPosition]);

  // Serial.print(", ");
  // Serial.print(_speedcomp.speedLowPass, 3);

  // Serial.print(", ");
  // Serial.print(_speedcomp._processInterval);

  // Serial.print(", ");
  // Serial.print(_speedcomp.speedLowPass - _plateau.targetRpm, 3);
  // Serial.print(", ");
  // Serial.print(_speedcomp.lowpassRect, 3);
  // Serial.print(", ");
  // Serial.print(_speedcomp.wow, 3);

  // Serial.print(", ");
  // Serial.print(_speedcomp.speedHighPass, 3);

  Serial.print(", ");
  Serial.print(_speedcomp.centerCompTargetRpm - _plateau.targetRpm, 3);

  // Serial.print(", ");
  // Serial.print(_speedcomp.rotationPosition / float(_speedcomp.pulsesPerRev));

  // Serial.print(", ");
  // Serial.print(_speedcomp.preComp, 4);

  Serial.print(", ");
  Serial.print(_speedcomp.unbalanceComp, 4);

  // Serial.print(", ");
  // Serial.print(_plateau._outBuff, 2);

  // Serial.print(", ");
  // Serial.print(_plateau._outBuffPrev, 2);

  // Serial.print(", ");
  // Serial.print(_arm.armAngleRaw); // 1696);

  // Serial.print(", ");
  // Serial.print(_carriage._Dcomp, 4); // 1696);
  Serial.print(", ");
  Serial.print(_arm.armAngleCall, 4); // 1696);

  // Serial.print(", ");
  // Serial.print(_arm.armAngleSlow, 5); // 1696);
  // Serial.print(", ");
  // Serial.print(_arm.armAngleOffset, 5); // 1696);

  // Serial.print(", ");
  // Serial.print(_carriage.position, 3);
  Serial.print(", ");
  Serial.print(_carriage.realPosition, 3);

  // // Serial.print(", ");
  // // Serial.print(_speedcomp.carriagePosMiddle, 3);

  // // Serial.print(", ");
  // // Serial.print(_speedcomp.carriagePosMiddle + _speedcomp.carriageFourier, 3);

  // Serial.print(", ");
  // Serial.print(_speedcomp.carriagePosMiddle + _speedcomp.carriageFourierFilter, 3);

  Serial.print(", ");
  Serial.print(_speedcomp.trackSpacing, 3);

  Serial.print(", ");
  Serial.print(_arm.weight, 3);

 Serial.println();
} // printGraphicData()


void SerialComm::report() {
  int padR = 25;
  Serial.println("-------------------- V" + String(_shared.version, 0) + " --------------------");
  Serial.println();
  // Serial.println(padRight("WHEEL_BOARD", padR) +            ": " + String(BOARD_DESCRIPTION));
  Serial.println(padRight("WHEEL_TEMPERATURE", padR) +      ": " + String(analogReadTemp(), 2) + " °C");
  // Serial.println(padRight("WHEEL_STATE", padR) +            ": " + getState(_shared.state));
  // Serial.println(padRight("WHEEL_VOLUME", padR) +           ": " + String(_amplifier.volume));
  // Serial.println(padRight("WHEEL_WIRELESS_VERSION", padR) + ": " + String(_bluetooth.wirelessVersion ? "YES" : "NO"));
  Serial.println();
  _storage.info();
  _orientation.info();
  // _speedcomp.info();
  Serial.println("----------------------------------------------");
} // report()


void SerialComm::info() {
  int padR = 25;
  Serial.println("-------------------- V" + String(_shared.version, 0) + " --------------------");
  Serial.println();
  Serial.println(padRight("WHEEL_BOARD", padR) +            ": " + String(BOARD_DESCRIPTION));
  Serial.println(padRight("WHEEL_TEMPERATURE", padR) +      ": " + String(analogReadTemp(), 2) + " °C");
  Serial.println(padRight("WHEEL_STATE", padR) +            ": " + getState(_shared.state));
  Serial.println(padRight("WHEEL_VOLUME", padR) +           ": " + String(_amplifier.volume));
  Serial.println(padRight("WHEEL_WIRELESS_VERSION", padR) + ": " + String(_bluetooth.wirelessVersion ? "YES" : "NO"));
  Serial.println();
  _storage.info();
  _orientation.info();
  _plateau.info();
  _speedcomp.info();
  _carriage.info();
  _scanner.info();
  _arm.info();
  _buttons.info();
  Serial.println("----------------------------------------------");
} // info()


//             bytes   cycles                
// LD (HL),d8      2   12
// INC L           1   4
//                 3   16

// LD (HL),d8      2   12
// INC HL          1   8
//                 3   20

// LD (HL+),A      1   8
// LD A,d8         2   8
//                 3   16                

// LD A, [DE]      1   8
// LD (HL+),A      1   8
//                 2   16

// LD SP,d16       3   12
// LD (a16),SP     3   20
//                 6   32
