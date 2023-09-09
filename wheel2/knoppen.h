#define KNOP_PLAY 2//~5  knop3
#define KNOP_DOORSPOEL 3//~4  knop2
#define KNOP_TERUGSPOEL 1//~6   knop4
#define KNOP_BT 7//~0     knop1
#define KNOP_RPM 0//~7    knop5

int potVal = 0;
int potValPrev = 0;

int potValPrevVoorVolume = 0;
int pot2volume = 100;

int knoppen[8];




INTERVAL knoppenInt(200, MILLIS);

void knoppenUpdate(){
  if(knoppenInt.loop()){
    for(int i = 0; i < 8; i++){
    
      knoppen[i] = digitalRead(displayUIT);

      // delayMicroseconds(1);
      digitalWrite(displayKLOK, 1);
      // delayMicroseconds(1);
      digitalWrite(displayKLOK, 0);
    }

    
    // Serial.print("knoppen= ");
    // Serial.print(volt2pwm(1 + (100 * 0.02)));
    // Serial.print("  ");

    // for(int i = 0; i < 8; i++){
    //   Serial.print(knoppen[i]);
    //   Serial.print(' ');
    // }

    // Serial.println();




    potVal = analogRead(displayPOTMETERanaloog);

    if(potVal - potValPrev   >   AMAX * 0.6){
      potValPrev += AMAX;
    }

    if(potVal - potValPrev   <   -AMAX * 0.6){
      potValPrev -= AMAX;
    }

    potValPrevVoorVolume    +=     potVal - potValPrev;

    potValPrev = potVal;

  }
}