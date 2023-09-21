#ifndef WHEEL_H
#define WHEEL_H

#include <Arduino.h>
#include "shared.h"
#include "amplifier.h"
#include "arm.h"
#include "bluetooth.h"
#include "buttons.h"
#include "carriage.h"
#include "display.h"
#include "orientation.h"
#include "plateau.h"
#include "scanner.h"
#include "serialcomm.h"
#include "speedcomp.h"
#include "storage.h"


class Wheel {
  public:
    Shared shared;
    Amplifier amplifier;
    Arm arm;
    Bluetooth bluetooth;
    Buttons buttons;
    Carriage carriage;
    Display display;
    Orientation orientation;
    Plateau plateau;
    Scanner scanner;
    SerialComm serialcomm;
    SpeedComp speedcomp;
    Storage storage;
    Wheel(float version);
    void init();
}; // Wheel


#endif // WHEEL_H
