#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

//kar stapper
#define stapperAN           8//~ 
#define stapperAP           9//~ 
#define stapperBN           10//~ 
#define stapperBP           11//~ 


// plateau encoder
#define plateauA            0//~ 
#define plateauB            1//~
#define plateauIndex        2//~

bool golven = true;


#include "interval.h"
#include "pwm.h"
#include "stappenMotor.h"
#include "compVaartSensor.h"

COMPVAART TLE5012(16, plateauA, 4096);

#include "serieel.h"



void setup() {
  Serial.begin();

  gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);
  gpio_set_irq_enabled_with_callback(plateauIndex,   GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);

  karInit();

  EEPROM.begin(4096);
}

void loop() {
  // put your main code here, to run repeatedly:
  karMotorFunc();
  serieelFunc();
}




void gpio_callback(uint gpio, uint32_t events) {
  // if(interruptBezig){return}
  // interruptBezig = true;

  if(gpio == plateauA){// || gpio == plateauB){
    TLE5012.interrupt();
  }
  
  if(gpio == plateauIndex){
    TLE5012.teller = 0;
    motorOffset = motorPos;
  }  

  // if(gpio == plaatStrobo){
  //   strobo.interrupt();
  // }

  // interruptBezig = false;
}