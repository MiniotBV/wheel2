#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "pwm.h"

#include "pins.h"
#include "helper.h"

#include "interval.h"

INTERVAL ledInt(200, MILLIS);




#include "versterker.h"

#include "armMotor.h"

#include "karStappenMotor.h"


#include "plaatLees.h"



float targetRpm = 33.333;

#include "vaartSensor.h"
#include "compVaartSensor.h"

COMPVAART TLE5012(64, plateauA, 4096);
// VAART strobo(8, 0, 180*2);

#include "knoppen.h"

#include "display.h"

#include "plateau.h"

#include "serieel.h"








void setup() {

  Serial.begin(115200);

  versterkerInit();

  displayInit();  

  armInit();

  karInit();

  plaatLeesInit();

  setPwm(ledWit);
  setPwm(ledRood);

  plateauInit();


  gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_FALL + GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);
  gpio_set_irq_enabled_with_callback(plateauB,   GPIO_IRQ_EDGE_FALL + GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);
  gpio_set_irq_enabled_with_callback(plateauIndex,   GPIO_IRQ_EDGE_FALL,  true,   &gpio_callback);

  // gpio_set_irq_enabled_with_callback(plaatStrobo,   GPIO_IRQ_EDGE_FALL + GPIO_IRQ_EDGE_RISE,  true,   &gpio_callback);

   multicore_launch_core1(core1_entry);
}






void core1_entry(){
  while(1){
    displayUpdate();

    knoppenUpdate();
  }
}


void loop() {

  armFunc();

  karMotorFunc();

  serieelFunc();

  volumeFunc();

  plateauFunc();



  pwmWrite(ledWit, pow( ((sin( (PI*millis()) / 500.0 )+1)/2), 3) * PMAX);
  // pwmWrite(ledRood, pow( ((cos( (PI*millis()) / 500.0 )+1)/2), 3) * PMAX);
}








// bool interruptBezig = false;


void gpio_callback(uint gpio, uint32_t events) {
  // if(interruptBezig){return}
  // interruptBezig = true;

  if(gpio == plateauA || gpio == plateauB){
    TLE5012.interrupt();
  }
  
  if(gpio == plateauIndex){
    TLE5012.teller = 0;
  }  

  // if(gpio == plaatStrobo){
  //   strobo.interrupt();
  // }

  // interruptBezig = false;
}

