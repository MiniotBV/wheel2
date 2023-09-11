#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "amplifier.h"
#include "arm.h"
#include "buttons.h"
#include "cart.h"
#include "orientation.h"
#include "plateau.h"
#include "scanner.h"
#include "speedcomp.h"
#include "storage.h"

#define DISPLAY_LENGTH 120

class Display {
  private:
    Interval _interval;
    Shared& _shared;
    Amplifier& _amplifier;
    Arm& _arm;
    Buttons& _buttons;
    Cart& _cart;
    Orientation& _orientation;
    Plateau& _plateau;
    Scanner& _scanner;
    SpeedComp& _speedcomp;
    Storage& _storage;
    int _trackCounter = 0;
    unsigned int _delay = 0;
    const int _dispHalf = DISPLAY_LENGTH / 2;
    float _data[DISPLAY_LENGTH];
    void clear();
    int mapRealPos2Display(float pos);
    void drawBlock(int start, int end, float color);
    void drawPoint(int pos, float color);
    void flipData();
    void print(float time);
    void commit();
  public:
    Display(Shared& shared, Amplifier& amplifier, Arm& arm, Buttons& buttons, Cart& cart,
      Orientation& orientation, Plateau& plateau, Scanner& scanner, SpeedComp& speedcomp,
      Storage& storage);
    void init();
    void update();
};

#endif
