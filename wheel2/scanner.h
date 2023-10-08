#ifndef SCANNER_H
#define SCANNER_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "plateau.h"

#define SCANNER_DETECTION_THRESHOLD 200


class Carriage; // pre-declare class


class Scanner {
  private:
    Interval _interval;
    Shared& _shared;
    Plateau _plateau;
    Carriage* _carriage;
    bool _cut;
    bool _trackBelowThreshold = true;
    bool _headerShown = false;
    float _recordPresentFiltered = 0;
    float _raw;
    float _rawPrev;
    float _rawDiff;
    float _absDiff;
    float _value;
    float _valuePrev;
    float _diff;
    float _currentP = 0.0006;
    float _sensorTarget = 1500;
    float _buffer[2000][2];
    int _bufferCounter = 0;
    int _bufferLength = 0;
    int _trackThreshold = 500;
    int _currentTrackPrev;
    void newTrack(float pos);
    void recordDetection();
    bool isRecordPresent();
    void scanForTracks();
    void clearTracks();
    int getCurrentTrack();
    void scanLedOff();
    void setLedMilliAmp(float amp);
    int volt2pwm(float volt);
    void printGraphicData();
  public:
    bool recordPresent = false;
    bool graphicData = false;
    float tracks[100];
    float recordStart = 0;
    float current = 10;
    int trackCount = 0;
    int currentTrack = 0;
    Scanner(Shared& shared, Plateau& plateau);
    void init(Carriage* carriage);
    void func();
    void check();
    void setTracksAs7inch();
    void info();
}; // Scanner


#endif // SCANNER_H
