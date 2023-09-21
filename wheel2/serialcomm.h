#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#define SERIAL_BAUDRATE 115400

#include <Arduino.h>
#include "enums.h"
#include "interval.h"
#include "shared.h"
#include "amplifier.h"
#include "arm.h"
#include "bluetooth.h"
#include "buttons.h"
#include "carriage.h"
#include "orientation.h"
#include "plateau.h"
#include "scanner.h"
#include "speedcomp.h"
#include "storage.h"


class SerialComm {
  private:
    Shared& _shared;
    Amplifier& _amplifier;
    Arm& _arm;
    Buttons& _buttons;
    Bluetooth& _bluetooth;
    Carriage& _carriage;
    Orientation& _orientation;
    Plateau& _plateau;
    Scanner& _scanner;
    SpeedComp& _speedcomp;
    Storage& _storage;
    Interval _interval;
    String _line = "";
    String _lineRaw = "";
    String _lastCommand = "";
    bool _graphicData = false;
    bool _headerShown = false;

    void checkReceivedLine(String line, eCheckMode mode = CM_NONE);
    bool checkLineCommand(String command, String description, eCheckMode mode);
    bool checkLine(String command, String description, eCheckMode mode);
    bool checkLineInt(String command, String description, eCheckMode mode, int& value);
    bool checkLineFloat(String command, String description, eCheckMode mode, float& value);
    bool checkLineBool(String command, String description, eCheckMode mode, bool& value);
    void println(eCheckMode mode);
    void printCommando(String command, String description);
    void printValue(String command, String description, String value);
    void printGraphicData();
    void report();
    void info();
  public:
    SerialComm(Shared& shared, Amplifier& amplifier, Arm& arm, Bluetooth& bluetooth, Buttons& buttons, Carriage& carriage,
      Orientation& orientation, Plateau& plateau, Scanner& scanner, SpeedComp& speedcomp, Storage& storage);
    void init();
    void func();
}; // SerialComm


#endif // SERIALCOMM_H
