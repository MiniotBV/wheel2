//Wheel2
//rp2040
#define versie 52


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "pico/time.h"






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




#include "opslag.h"



#include "serieel.h"

// ====================================================
//  HARDWARE TIMER TEST
// ====================================================

#include <stdio.h>
#include "pico/stdlib.h"
 
volatile bool timer_fired = false;
 
int64_t alarm_callback(alarm_id_t id, void *user_data) {
    Serial.print("Timer fired ");
    Serial.println( (int) id);
    timer_fired = true;
    return 0; // Can return a value here in us to fire in the future
}
 
bool repeating_timer_callback(struct repeating_timer *t) 
{
    Serial.print("Repeat at: ");
    Serial.println(time_us_64());
    return true;
}
 
// int main() {
//     // stdio_init_all();
//     // printf("Hello Timer!\n");
 
//     // Call alarm_callback in 2 seconds
//     add_alarm_in_ms(2000, alarm_callback, NULL, false);
 
//     // Wait for alarm callback to set timer_fired
//     while (!timer_fired) {
//         tight_loop_contents();
//     }
 
//     // Create a repeating timer that calls repeating_timer_callback.
//     // If the delay is > 0 then this is the delay between the previous callback ending and the next starting.
//     // If the delay is negative (see below) then the next call to the callback will be exactly 500ms after the
//     // start of the call to the last callback
//     struct repeating_timer timer;
//     add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer);
//     sleep_ms(3000);
//     bool cancelled = cancel_repeating_timer(&timer);
//     printf("cancelled... %d\n", cancelled);
//     sleep_ms(2000);
 
//     // Negative delay so means we will call repeating_timer_callback, and call it again
//     // 500ms later regardless of how long the callback took to execute
//     add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);
//     sleep_ms(3000);
//     cancelled = cancel_repeating_timer(&timer);
//     printf("cancelled... %d\n", cancelled);
//     sleep_ms(2000);
//     printf("Done\n");
//     return 0;
// }



// static repeating_timer_t timers[10] = {0,};

// repeating_timer_t testt;
// testt.callback = &testCallback;

// bool testCallback()
// {
//   Serial.println("hallo: " + String(micros()));
// }

// bool testt.repeating_timer_callback_t(){  //} *callback() {
//   Serial.println("hallo: " + String(micros()));
//   return testt;
// }




void setup() {
  analogReadResolution(12);

  Serial.begin(115200);

  opslagInit();

  eepromUitlezen();
  
// ====================================================
//  HARDWARE TIMER TEST
// ====================================================

//https://raspberrypi.github.io/pico-sdk-doxygen/group__repeating__timer.html

  // add_repeating_timer_ms( 2000, callback, &timers[1], testt );
  
  // Call alarm_callback in 2 seconds
    add_alarm_in_ms(2000, alarm_callback, NULL, false);
 
    // Wait for alarm callback to set timer_fired
    while (!timer_fired) {
        tight_loop_contents();
    }
 
    // Create a repeating timer that calls repeating_timer_callback.
    // If the delay is > 0 then this is the delay between the previous callback ending and the next starting.
    // If the delay is negative (see below) then the next call to the callback will be exactly 500ms after the
    // start of the call to the last callback
    struct repeating_timer timer;
    add_repeating_timer_us(500, repeating_timer_callback, NULL, &timer);
    sleep_ms(3000);
    bool cancelled = cancel_repeating_timer(&timer);
    Serial.print("cancelled... "); Serial.println(cancelled);
    sleep_ms(2000);
 
    // Negative delay so means we will call repeating_timer_callback, and call it again
    // 500ms later regardless of how long the callback took to execute
    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);
    sleep_ms(3000);
    cancelled = cancel_repeating_timer(&timer);
    Serial.print("cancelled..."); Serial.println(cancelled);
    sleep_ms(2000);
    // Done
// ====================================================
//  End HARDWARE TIMER TEST
// ====================================================



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

    if(eepromShit > 0){
      eepromShit = 2;
      delay(100);
    }
  }
}






void loop() {
  if(enkeleCoreModus){
    core1Dingen();    
  }


  if(eepromShit){
    delay(20);
    // while(eepromShit != 2){}
    // int tijd = micros();
    eepCommit();
    // Serial.println("gelukt in: " + String(micros() - tijd));
    Serial.println("opgelagen!");
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

