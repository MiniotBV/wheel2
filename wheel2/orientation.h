#ifndef ORIENTATION_H
#define ORIENTATION_H

/**
Makes use of QMA7981, a Single-Chip 3-Axis Accelerometer
https://datasheet.lcsc.com/lcsc/2004281102_QST-QMA7981_C457290.pdf

I2C Slave Address: b0010010
*/

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "arm.h"

class Orientation {
  private:
    Interval _interval;
    Interval _isOkInterval;
    Shared& _shared;
    Arm& _arm;
    const byte _i2cAdress = 0b0010010;
    bool _firstTime = true;
    bool _error = false;
    bool _errorPrev = false;
    bool _isStandingPrev = false;
    bool _headerShown = false;
    float _rawX, _rawY, _rawZ;
    void printGraphicData();
  public:
    bool graphicData = false;
    bool isStanding = false;
    float x, y, z;
    float offsetX = 0;
    float offsetY = 0;
    float offsetZ = 0;
    Orientation(Shared& shared, Arm& arm);
    void init();
    void calibrate();
    void reset();
    void update();
    void info();
}; // Orientation

#endif
