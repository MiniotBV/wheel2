#include <stdio.h>
// #include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "pins.h"
#include "helper.h"

#include "pwm.h"

#include "armMotor.h"

#include "karStappenMotor.h"







float targetRpm = 33.333;

#include "vaartSensor.h"
#include "compVaartSensor.h"

COMPVAART TLE5012(64, plateauA, 4096);
VAART strobo(8, 0, 180*2);


#include "plateau.h"



#include "serieel.h"






void setup() {

  Serial.begin(115200);

  setPwm(armMotor);

  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);

  setPwm(led);

  setPwm(motorP);
  pwmWrite(motorP, 0);

  setPwm(motorN);
  pwmWrite(motorN, 0);


  gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_FALL,  true,   &gpio_callback);
  gpio_set_irq_enabled_with_callback(plateauIndex,   GPIO_IRQ_EDGE_FALL,  true,   &gpio_callback);

  gpio_set_irq_enabled_with_callback(plaatStrobo,   GPIO_IRQ_EDGE_FALL + GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);
}










void loop() {

  armFunc();

  karMotorFunc();

  serieelFunc();

  plateauFunc();

  pwmWrite(led, pow( ((sin( (PI*millis()) / 500.0 )+1)/2), 3) * PMAX);
}











void gpio_callback(uint gpio, uint32_t events) {
  if(gpio == plateauA){
    TLE5012.interrupt();
  }
  if(gpio == plateauIndex){
    call = TLE5012.teller;
    TLE5012.teller = 0;
  }  

  if(gpio == plaatStrobo){
    strobo.interrupt();
  }
}

