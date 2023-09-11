// motor has 48 steps
// gear has 10 teeth
// rack-and-pinion has 1.5mm tooth pitch 
// 48 steps / 12 teeth = 4 steps per tooth
// 1.5mm / 4 steps per tooth = 0.375mm per step
// PI = 2 steps

#ifndef CART_H
#define CART_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "arm.h"
#include "plateau.h"
#include "scanner.h"

#define CART_HOME 44
#define CART_PARK CART_HOME - 1.5 // 2.5 //2
#define CART_CLEAN_POS 75 

#define CART_12INCH_START 146.5 //147
#define CART_10INCH_START 124 //125
#define CART_7INCH_START 84 //85
#define CART_RECORD_END 52.5

#define CART_BACKTRACK_OFFSET 1 // how many mm the cart can move before skipping to start track instead of previous track 
#define CART_MAX_SPEED 0.02
#define CART_ACCELERATION 0.0001

#define CART_SENSOR_OFFSET 7.5 //mm

class SpeedComp;

class Cart {
  private:
    Interval _interval;
    Shared& _shared;
    Arm& _arm;
    Plateau& _plateau;
    Scanner& _scanner;
    SpeedComp* _speedcomp;
    bool _motorEnable = true;
    bool _headerShown = false;
    const int _stepperGearTeeth = 12; // 8;
    const float _mmPerStep = 1.5 / ( 48 / _stepperGearTeeth );
    const float _step2mm = ( 2 / PI ) * _mmPerStep; // 0.238732414637843
    const float _mm2step = 1 / _step2mm;
    double _speed = 0;
    float _Dcomp = 0;
    float _offCenterCompFilter;
    float _motorPos = 0;
    float _offset = 0; // for homing deviation
    float _newPosition;
    float _acceleration = 0;
    float _distanceToStop = 0;
    void gotoTrack(float pos);
    void gotoRecordStart();
    bool movetoPosition(float target, float spd);
    void stopOrRepeat();
    void stateUpdate();
    bool decelerate();
    void emergencyStop();
    void printGraphicData();
  public:
    Interval movedForwardInterval;
    bool offCenterCompensation = true;
    bool graphicData = false;
    bool repeat = false;
    float P = 2; // 4; //0.001; //0.0005; //0.00005; //0.00025;
    float I = 0; // 0.005; //0.00005; //0.00025;
    float D = 1; // 2; //1.5; //1.5; //0.0006; //-0.003;
    float targetTrack = 0;
    float trackOffset = 0.3; //0.7;
    float position = CART_HOME;
    float positionFilter = CART_HOME;
    float realPosition = CART_HOME;
    float sensorPosition;
    Cart(Shared& shared, Arm& arm, Plateau& plateau, Scanner& scanner);
    void init(SpeedComp* speedcomp);
    void func();
    void gotoNextTrack();
    void gotoPreviousTrack();
    void pause();
    void info();
};

#endif
