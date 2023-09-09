#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <EEPROM.h>
// #include <LittleFS.h>
#include "pwm.h"
#include "pins.h"
#include "helper.h"
#include "interval.h"
#include "armMotor.h"
Arm arm;
// bool eepromPauze = false;
#include "w2_stroboSensor.h"  //#include "vaartSensor.h"
// StroboSpeed strobo(12, (60 / rpm33) * 1000); //1800
// StroboSpeed strobo(24, (60 / rpm33) * 1000); //1800
// StroboSpeed strobo(7*2, (60 / rpm33) * 1000 * 2); //1800
StroboSpeed strobo(14, 1800); //1800

#include "compVaartSensor.h"
// COMPVAART TLE5012(16, 4096); //elker 5ms is 11.4 samples en 22.75 per 10ms
COMPVAART TLE5012(64, 8192); //elker 5ms is 11.4 samples en 22.75 per 10ms

#include "staat.h"
#include "plateau.h"
Interval ledInt(200, inMILLIS);

#include "w2_amplifier.h"   //#include "versterker.h"
Amplifier amp;
#include "w2_carMotor.h"   //#include "karStappenMotor.h"
CarMotor car;
#include "plaatLees.h"
#include "knoppen.h"
#include "w2_display.h"
Display display;
#include "serieel.h"
// ==================================================
//    Single/Double core stuff
// ==================================================
void loop2();
bool enkeleCoreModus = true;

void uitEnkeleCoreModus()
{
  enkeleCoreModus = false;
  multicore_launch_core1(loop2);
}
// ==================================================
//    Interrupt stuff
// ==================================================
bool encoderBezig = false;
bool toonBezig = false;
void gpio_callback(uint gpio, uint32_t events)
{
  // if(interruptBezig){return}
  // interruptBezig = true;
  if(!encoderBezig){
    if( gpio == plateauA){// || gpio == plateauB){
      encoderBezig = true;
      TLE5012.interrupt();
      encoderBezig = false;
    }
  }
  
  if(gpio == plateauIndex) { TLE5012.teller = 0; }  

  if(gpio == audioFreqPin) { strobo.interrupt(); }

  //   if(gpio == plaatStrobo){
  //   strobo.interrupt();
  // }

  // interruptBezig = false;
}
void enableInterupts(bool aan)
{
    gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
    // gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
  // gpio_set_irq_enabled_with_callback(plateauB,   GPIO_IRQ_EDGE_FALL + GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);
    gpio_set_irq_enabled_with_callback(plateauIndex,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);

    pinMode(audioFreqPin, INPUT);
    // gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
    gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_RISE,  aan,   &gpio_callback);
    // gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
}
