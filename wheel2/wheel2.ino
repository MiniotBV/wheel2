#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "pwm.h"

#include "pins.h"
#include "helper.h"

#include "interval.h"

#include "armMotor.h"

bool eepromPauze = false;

void enableInterupts(bool aan){
    gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
    // gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
  // gpio_set_irq_enabled_with_callback(plateauB,   GPIO_IRQ_EDGE_FALL + GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);
    gpio_set_irq_enabled_with_callback(plateauIndex,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);

    pinMode(audioFreqPin, INPUT);
    // gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
    gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_RISE,  aan,   &gpio_callback);
    // gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
}

#include "vaartSensor.h"
VAART strobo(7, (60 / rpm33) * 1000); //1800
// VAART strobo(7*2, (60 / rpm33) * 1000 * 2); //1800

#include "compVaartSensor.h"
// COMPVAART TLE5012(16, 4096); //elker 5ms is 11.4 samples en 22.75 per 10ms
COMPVAART TLE5012(64, 8192); //elker 5ms is 11.4 samples en 22.75 per 10ms




#include "staat.h"

#include "plateau.h"

Interval ledInt(200, MILLIS);




#include "versterker.h"

#include "karStappenMotor.h"

#include "plaatLees.h"






#include "knoppen.h"

#include "display.h"

#include "serieel.h"








void setup() {

  Serial.begin(115200);

  EEPROM.begin(4096);
  // TLE5012.recalCompSamples();

  versterkerInit();

  displayInit();  

  armInit();

  karInit();

  plaatLeesInit();

  setPwm(ledWit);
  setPwm(ledRood);

  plateauInit();

  delay(1);
  
  enableInterupts(true);

  multicore_launch_core1(loop2);

  stoppen();
}








void loop2(){
  

  while(1){
    if(eepromPauze){
      delay(1);
    }

    displayUpdate();
  
    serieelFunc();
  
    knoppenUpdate();

    armFunc();
    
  }
}




void loop() {

  plaatLeesFunc();

  karMotorFunc();

  volumeFunc();

  plateauFunc();

  // staatFunc();

  // pwmWrite(ledWit, pow( ((sin( (PI*millis()) / 500.0 )+1)/2), 3) * PMAX);
  pwmWriteF(ledWit, 0.5);
  // pwmWrite(ledRood, pow( ((cos( (PI*millis()) / 500.0 )+1)/2), 3) * PMAX);

  pwmWriteF(ledRood, plaatAanwezig ? 1 : 0);
}








// bool interruptBezig = false;


void gpio_callback(uint gpio, uint32_t events) {
  // if(interruptBezig){return}
  // interruptBezig = true;

  if(gpio == plateauA){// || gpio == plateauB){
    TLE5012.interrupt();
  }
  
  if(gpio == plateauIndex){
    TLE5012.teller = 0;
  }  

  if(gpio == audioFreqPin){
    strobo.interrupt();
  }

  //   if(gpio == plaatStrobo){
  //   strobo.interrupt();
  // }

  // interruptBezig = false;
}

