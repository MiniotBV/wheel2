// ==================================================
//  Wheel2 - main board 
//  processor: RP2040
//  date: dec 2021 
// ==================================================
const float softwareVersion = 0.171f;
#include "w2_global.h"

// ==================================================
//    Setup
// ==================================================
void setup() 
{
  Serial.begin(115200);
  EEPROM.begin(4096);
  amp.init();         // versterkerInit();
  display.init();     // displayInit();  
  arm.armInit();
  car.init();         // karInit();
  plaatLeesInit();
  setPwm(ledWit);
  setPwm(ledRood);
  plateauInit();
  delay(1);
  TLE5012.recalCompSamples();
  enableInterupts(true);

  getKnopData();
  if(knopIn[KNOP_RPM] == 0){ //als rpm knop niet is ingedrukt bij start up gaat alles normaal
    uitEnkeleCoreModus();
  }
  

  stoppen();
}

void core1Dingen()
{
  display.update();     //displayUpdate();
  serieelFunc();
  knoppenUpdate();
  arm.update();
}

void loop2()
{
  while(1) { core1Dingen(); }
}
// ==================================================
//    Main loop
// ==================================================
void loop()
{

  if(enkeleCoreModus) { core1Dingen(); }
  plaatLeesFunc();
  car.update();     // karMotorFunc();
  amp.update();     // volumeFunc();
  plateauFunc();
  // staatFunc();
  pwmWriteF(ledWit, pow( ((sin( (PI*millis()) / 500.0 )+1)/2), 3));
  // pwmWriteF(ledWit, 0.5);
  // pwmWrite(ledRood, pow( ((cos( (PI*millis()) / 500.0 )+1)/2), 3) * PMAX);
  pwmWriteF(ledRood, car.plaatAanwezig ? 1 : 0);
}
