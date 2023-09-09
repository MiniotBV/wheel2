//Wheel2
//rp2040
#define versie 52


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"


#include "opslag.h"





#include "pwm.h"

#include "pins.h"
#include "helper.h"

#include "interval.h"


#include "staat.h"

#include "armMotor.h"


bool enkeleCoreModus = true;
bool audioFequencyMeten = false;

void uitEnkeleCoreModus(){
  enkeleCoreModus = false;
  multicore_launch_core1(loop2);
}



void enableInterupts(bool aan){
  // pinMode(plateauA,     INPUT_PULLUP);
  // pinMode(plateauB,     INPUT_PULLUP);
  // pinMode(plateauIndex, INPUT_PULLUP);

  gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
    // gpio_set_irq_enabled_with_callback(plateauA,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
  gpio_set_irq_enabled_with_callback(plateauB,   GPIO_IRQ_EDGE_RISE + GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
  gpio_set_irq_enabled_with_callback(plateauIndex,   GPIO_IRQ_EDGE_FALL,  aan,   &gpio_callback);
}

void toggleAudioFreqMeting(){
  audioFequencyMeten = !audioFequencyMeten;
  
  pinMode(audioFreqPin, INPUT);
  gpio_set_irq_enabled_with_callback(audioFreqPin,   GPIO_IRQ_EDGE_RISE,  audioFequencyMeten,   &gpio_callback);
}





#include "vaartSensor.h"
// VAART calibratieToon(12, (60 / rpm33) * 1000); //1800
// VAART calibratieToon(24, (60 / rpm33) * 1000); //1800
// VAART calibratieToon(7*2, (60 / rpm33) * 1000 * 2); //1800
VAART calibratieToon(10, 1800); //1800

#include "compVaartSensor.h"
// COMPVAART strobo(2, 1024); //elker 5ms is 11.4 samples en 22.75 per 10ms
// COMPVAART strobo(16, 4096); //elker 5ms is 11.4 samples en 22.75 per 10ms
// COMPVAART strobo(64, 8192); //elker 5ms is 11.4 samples en 22.75 per 10ms

// COMPVAART strobo(8, 720);//360); //elker 5ms is 11.4 samples en 22.75 per 10ms
COMPVAART strobo(32, 720);//360); //elker 5ms is 11.4 samples en 22.75 per 10ms





#include "plateau.h"

Interval ledInt(200, MILLIS);




#include "versterker.h"

#include "kar.h"

#include "plaatLees.h"






#include "knoppen.h"

#include "display.h"

#include "serieel.h"




// static repeating_timer_t timers[10] = {0,};



// void callback(void) {
//   Serial.println("hallo: " + String(micros()));
// }

void setup() {
  analogReadResolution(12);

  Serial.begin(115200);

  opslagInit();
  

  // add_repeating_timer_ms( 2000, callback, &timers[1] );


  versterkerInit();

  displayInit();  

  armInit();

  karInit();

  plaatLeesInit();

  setPwm(ledWit);

  plateauInit();
  
  enableInterupts(true);

  getKnopData();
  // if(knopIn[KNOP_RPM] == 0){ //als rpm knop niet is ingedrukt bij start up gaat alles normaal
  if(knopIn[KNOP_TERUGSPOEL] == 0){ //als rpm knop niet is ingedrukt bij start up gaat alles normaal
    uitEnkeleCoreModus();
  }
  
  pinMode(slaapStand, OUTPUT);
  digitalWrite(slaapStand, 1); // hou de batterij aan
  // stoppen();

}








void core1Dingen(){
  displayUpdate();
  
  serieelFunc();

  knoppenUpdate();

  armFunc();  
}




void loop2(){
  while(1){
    core1Dingen();

    if(eepromShit){
      delay(100);
      eepromShit = false;
    }
  }
}






void loop() {
  if(enkeleCoreModus){
    core1Dingen();    
  }


  if(eepromShit){
    eepCommit();
    Serial.println("doei");
    delay(100);
  }



  plaatLeesFunc();

  karMotorFunc();

  volumeFunc();

  plateauFunc();

  // staatFunc(); // zit in karFunc()

  pwmWriteF(ledWit, plaatAanwezigGefilterd);
}







void gpio_callback(uint gpio, uint32_t events) {
  if( gpio == plateauA || gpio == plateauB){
    strobo.interrupt();
  }
  
  if(gpio == plateauIndex){
    strobo.teller = 0;
  }  

  if(gpio == audioFreqPin){
    calibratieToon.interrupt();
  }
}

