// 卐 Wheel2 卐
//rp2040
#define versie 27


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"



#include "pwm.h"

#include "pins.h"
#include "helper.h"

#include "interval.h"



#include "kar.h"




#include "serieel.h"








void setup() {

  Serial.begin(115200);

  


  karInit();

  // setPwm(ledWit);


}














void loop() {

  serieelFunc();



  karMotorFunc();



  // staatFunc(); // zit in karFunc()

  // pwmWriteF(ledWit, pow( ((sin( (PI*millis()) / 500.0 )+1)/2), 3));
    // pwmWriteF(ledWit, pow( ((sin( (PI*millis()) / 500.0 )+1)/2), 3));
  // pwmWriteF(ledWit, 0.5);
}







