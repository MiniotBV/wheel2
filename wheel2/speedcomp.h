#ifndef SPEEDCOMP_H
#define SPEEDCOMP_H

#include <Arduino.h>
#include "shared.h"
#include "arm.h"

#define SPEEDCOMP_TAU         2*PI

#define SPEEDCOMP_SAMPLES     16
#define SPEEDCOMP_PPR         720
#define SPEEDCOMP_SAMPLES_MAX 65500
#define SPEEDCOMP_PPR_MAX     1000

class Cart; // pre-declare class
class Plateau; // pre-declare class

class SpeedComp {
  private:
    Shared& _shared;
    Arm& _arm;
    Cart* _cart;
    Plateau* _plateau;

    //-------------------- speed
    volatile unsigned int _speedInterval;
    volatile unsigned int _time;
    volatile unsigned int _interval;

    //-------------------- direction
    char _sens, _sensPrev;
    int _direction;
    int _directionPrev;
    int _glitchCounter;
    int _counterRaw = 0;

    //-------------------- samplede sinus & cos
    float _sinus[SPEEDCOMP_PPR_MAX];
    float _cosin[SPEEDCOMP_PPR_MAX];

    //-------------------- filter
    volatile int _samplesArr[100];
    volatile unsigned int _sampleCounter = 0;

    float _average = SPEEDCOMP_SAMPLES_MAX;

    //-------------------- off center compensation
    float _cartPosMiddlePre;
    float _cartOffCenterWave[SPEEDCOMP_PPR_MAX];
    float _cartPosCenterHist[SPEEDCOMP_PPR_MAX];
    float _cartSinValues[SPEEDCOMP_PPR_MAX];
    float _cartCosValues[SPEEDCOMP_PPR_MAX];
    float _cartSin;
    float _cartCos;

    float _cartSinFilt;
    float _cartCosFilt;
    float _centerComp;

    //-------------------- unbalance compensation
    int _unbalanceFilterCurve[SPEEDCOMP_PPR_MAX];
    int _unbalansComp[SPEEDCOMP_PPR_MAX];
    int _unbalanceFilterCurveWidth = 0;

    //-------------------- debug
    unsigned int _processTime;
    unsigned int _processInterval;
    bool _clearCompSamplesQueue = false;
    bool _headerShown = false;
    bool _wowFirstLow;
    int _counterSinceReset;

    void clearSamples();
    void clearUnbalanceCompSamples();
    void clearCenterCompSamples();
    float getSpeed();
    float averageInterval();
    float currentSpeed(float inter);
    void shiftSamples(int sample);
    void printGraphicData();
  public:
    //-------------------- direction
    int counter = 0;
    int pulsesPerRev = SPEEDCOMP_PPR;

    //-------------------- filter
    int samples = SPEEDCOMP_SAMPLES;
    float speedRaw;
    float speed;
    float speedCenterComp;
    float speedLowPass;
    float speedHighPass;
    float lowpassRect;
    float wow;

    //-------------------- off center compensation
    float cartFourier;
    float cartFourierFilter;

    //-------------------- unbalance compensation
    int unbalancePhase = 23; //25; //50; //50;
    float unbalanceCompWeight = 1.2; // 1.1; //1.3; //2;
    float unbalanceFilterWidth = 80; //65; //50; //100;
    volatile float unbalanceComp = 0;

    //-------------------- debug
    bool graphicData = false;
    bool unbalanceCompOn = true;
    bool recordOffCenterComp = true;

    float cartPosMiddle;
    float trackSpacing;
    float centerCompTargetRpm;

    SpeedComp(Shared& shared, Arm& arm);
    void init(Cart* cart, Plateau* plateau);
    void update();
    void clearCompSamples();
    void clearCompSamplesOnT0();
    void stroboInterrupt();
    void createUnbalanceFilterCurve();
    void info();
}; // SpeedComp

#endif
