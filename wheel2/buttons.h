#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "amplifier.h"
#include "arm.h"
#include "bluetooth.h"
#include "carriage.h"
#include "orientation.h"
#include "plateau.h"
#include "scanner.h"

#define BUTTON_COUNT            8

#define BUTTON_NEXT             5
#define BUTTON_PLAY             6
#define BUTTON_PREV             7

#define BUTTON_PRESS            1
#define BUTTON_LONG_PRESS       2
#define BUTTON_SUPERLONG_PRESS  3
#define BUTTON_RELEASE          0

#define BUTTON_LONG_CLICK       700
#define BUTTON_SUPERLONG_CLICK  3000


class Buttons {
  private:
    Shared& _shared;
    Amplifier& _amplifier;
    Arm& _arm;
    Bluetooth& _bluetooth;
    Carriage& _carriage;
    Orientation& _orientation;
    Plateau& _plateau;
    Scanner& _scanner;
    Interval _interval;
    Interval _allButtonsInterval;
    int _buttonIn[BUTTON_COUNT];
    unsigned long _buttonInterval[BUTTON_COUNT];
    int potVal = 0;
    int potValPrev = 0;
    float belt = 0;
    float beltPrev = 0;
    float beltFilter;
    float beltFilterPrev;
    float beltDiff;
    void readData();
    void logic(int button);
    void ledBlink();
    bool isButtonNext(int button);
    bool isButtonPrev(int button);
    int buttonNextComp();
    int buttonPrevComp();
    void log(int button, String action);
    String getButton(int button);
  public:
    Interval rpmDisplayActionInterval;
    Interval volumeDisplayActionInterval;
    Interval ledBlinkInterval;
    int state[BUTTON_COUNT];
    Buttons(Shared& shared, Amplifier& amplifier, Arm& arm, Bluetooth& bluetooth, Carriage& carriage, Orientation& orientation, Plateau& plateau, Scanner& scanner);
    void init();
    void update();
    void info();
};


#endif // BUTTONS_H
