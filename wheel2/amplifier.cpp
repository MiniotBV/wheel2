#include "log.h"
#include "amplifier.h"
#include "pins.h"
#include "i2c.h"

Amplifier::Amplifier(Shared& shared, Arm& arm) :
  _shared(shared),
  _arm(arm),
  _interval(20, TM_MILLIS) {
} // Amplifier()

void Amplifier::init() {
  LOG_DEBUG("amplifier.cpp", "[init]");
  pinMode(AMP_HEADSET_EN_PIN, OUTPUT);
  digitalWrite(AMP_HEADSET_EN_PIN, 1);
} // init()

void Amplifier::func() {
  if (_interval.tick()) {
    if (!isNeedeDownLongEnough() && !volumeOverRide) {
      digitalWrite(AMP_HEADSET_EN_PIN, 0);
      _volumePrev = -88; // to force resend trigger
      // LOG_DEBUG("amplifier.cpp", "[volumeFunc] Sound off");
      return;
    }

    if (volume != _volumePrev || isNeedeDownLongEnough() != _isNeedleDownPrev || volumeOverRide != _volumeOverRidePrev) {
      digitalWrite(AMP_HEADSET_EN_PIN, 1);

      _volumePrev = volume;
      _isNeedleDownPrev = isNeedeDownLongEnough();
      _volumeOverRidePrev = volumeOverRide;

      //0b11000000); // stereo
      //0b11010000); // left in mono headset
      //0b11100000); // left in bridge-tied speaker
      int error = 0;
      error = i2cWrite(0x60, 1, 0b11000000);

      int value = volume;
      error = i2cWrite(0x60, 2, byte(value));

      if (error) {
        // LOG_ALERT("amplifier.cpp", "[volumeFunc] No headset amplifier");
        Serial.println("No headset amplifier");
      } else {
        Serial.println("VOLUME: " + String(volume));
      }
    }
  } // _interval.tick()
} // func()

bool Amplifier::isNeedeDownLongEnough() {
  // no sound when in puristMode
  return _arm.isNeedleDownFor(2000) && _shared.state == S_PLAYING && !_shared.puristMode;
} // isNeedeDownLongEnough
