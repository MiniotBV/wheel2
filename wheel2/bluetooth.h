#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "carriage.h"
#include "plateau.h"

#define BT_WIRELESS_VERSION false;

//------------------------------ command starts with this character
#define BT_BUTTON_IN      "4"
#define BT_BUTTON_OUT     "c"

//------------------------------ command ends with this character
#define BT_NEXT_TRACK     "b"
#define BT_PREV_TRACK     "c"
#define BT_SKIP_FORWARD   "9"
#define BT_SKIP_REVERSE   "8"

#define BT_PLAY           "4"   // headset on
#define BT_PAUSE          "6"  // headset off?

// #define BT_PAUZE_LANG

//AT+
//AT+SCAN
//AT+REST
//AT+DELVMLINK

class Bluetooth {
  private:
    Interval _interval;
    Interval _checkBeforeStartInterval;
    Shared& _shared;
    Carriage& _carriage;
    Plateau& _plateau;
    bool _initTodo = true;
    bool _wirelessVersion = false;
    String _buffer = "";
    void encode();
  public:
    const bool wirelessVersion = BT_WIRELESS_VERSION;
    bool debug = false;
    Bluetooth(Shared& shared, Carriage& carriage, Plateau& plateau);
    void init();
    void func();
    void write(String command);
}; // Bluetooth

#endif
