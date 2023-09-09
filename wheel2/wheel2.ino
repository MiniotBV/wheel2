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

#include "compVaartSensor.h"

COMPVAART TLE5012(16, plateauA, 4096);
// VAART strobo(8, 0, 180*2);



#include "staat.h"

#include "plateau.h"

Interval ledInt(200, MILLIS);




#include "versterker.h"

#include "karStappenMotor.h"

#include "plaatLees.h"



#include "vaartSensor.h"



#include "knoppen.h"

#include "display.h"

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


  stoppen();
}






void core1_entry(){
  while(1){
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

  pwmWrite(ledRood, plaatAanwezig ? (PMAX-1) : 0);
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

