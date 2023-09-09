#define KNOP_PLAY 2//~5  knop3
#define KNOP_DOORSPOEL 3//~4  knop2
#define KNOP_TERUGSPOEL 1//~6   knop4
#define KNOP_BT 7//~0     knop1
#define KNOP_RPM 0//~7    knop5


#define KNOP_LANG 700  //wat is een lange klik
#define KNOP_SUPER_LANG 3000  //een super lange klink voor naald stand
#define KNOP_BLINK 100 //hoelang blinkt de stick led



#define INGEDRUKT             1
#define LANG_INGEDRUKT        2
#define SUPER_LANG_INGEDRUKT  3
#define LOSGELATEN            0


int           knopIn[8];
int           knopOud[8];
int           knopStaat[8];
unsigned long knopInterval[8];
unsigned long knopAlleInterval;
unsigned long ledBlinkInterval;





int potVal = 0;
int potValPrev = 0;

float potVolume = 0;
float potVolumePrev = 0;
float potVolumeDiv;
float potVolumeFilter;
float potVolumeFilterPrev;
int pot2volume = 100;




void printKnoppen(){
  Serial.print("knoppen= ");
  for(int i = 0; i < 8; i++){
    Serial.print(knopIn[i]);
    Serial.print(' ');
  }
  Serial.println();
}



void ledBlink(){
  ledBlinkInterval = millis();
}

const char* knopNaam(int knop){
  if(knop == KNOP_PLAY){
    return "play";
  }
  if(knop == KNOP_DOORSPOEL){
    return "doorspoel";
  }
  if(knop == KNOP_TERUGSPOEL){
    return "terugspoel";
  }
  if(knop == KNOP_BT){
    return "bt";
  }
  if(knop == KNOP_RPM){
    return "rpm";
  }

  return "?";
}














Interval knoppenInt(10000, MICROS);

void knoppenUpdate(){
  if(knoppenInt.loop()){

    gpio_put(displayLATCH, 0);
    gpio_put(displayLATCH, 1);

    for(int i = 0; i < 8; i++){
    
      knopIn[i] = digitalRead(displayUIT);

      digitalWrite(displayKLOK, 1);
      // delayMicroseconds(1);
      digitalWrite(displayKLOK, 0);
      // delayMicroseconds(1);
    }

    

    // bool zelfde = false;

    // for(int i = 0; i < 8; i++){
      
    //   if(knopOud[i] != knopIn[i]){
    //     zelfde = true;
    //   }
    //   knopOud[i] = knopIn[i];

    // }

    // if(zelfde){
    //   printKnoppen();
    // }






    for(int knop = 0; knop < 8; knop++){

      
      if(       knopStaat[knop] == LOSGELATEN   &&    knopIn[knop] == INGEDRUKT ){//             KORT INGEDRUKT
        knopStaat[knop] = INGEDRUKT;
        
        knopAlleInterval = millis();
        knopInterval[knop] = millis();
        
        // Serial.print(knopNaam( knop));Serial.println(" in ");
        
        if( staat == S_SCHOONMAAK_BEWEGEN  ||  staat == S_SCHOONMAAK ){//   SCHOONMAAK STAND STOPPEN
          stoppen();
          ledBlink();  //led blink
        }
      }
      
      

      
      else if( knopStaat[knop] == INGEDRUKT    &&    knopIn[knop] == LOSGELATEN ){//               KORT LOSGELATEN
        knopStaat[knop] = LOSGELATEN;
        knopAlleInterval = millis();
        
        // Serial.print(knopNaam( knop));Serial.println(" los ");

        if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)   &&   knop == KNOP_DOORSPOEL){
          naarVolgendNummer();
          ledBlink();  //led blink
        }

        if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)   &&   knop == KNOP_TERUGSPOEL){
          naarVorrigNummer();
          ledBlink();  //led blink
        }


        
        if(knop == KNOP_PLAY){
          if(staat == S_PAUZE  ||  staat == S_NAALD_EROP){
            pauze();
            ledBlink();  //led blink
          


          }else if(staat == S_HOK){
            spelen();
            ledBlink();  //led blink
          }
        }
      }
      
      
      
      
      else if( knopStaat[knop] == INGEDRUKT    &&    millis() - knopInterval[knop]  >  KNOP_LANG ){//               LANG INGEDRUKT
        knopStaat[knop] = LANG_INGEDRUKT;
        knopAlleInterval = millis();

        // Serial.print(knopNaam( knop));Serial.println(" lang ");


        
        
        if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)  &&   knop == KNOP_DOORSPOEL){ //      >> DOOR SPOELEN
          setStaat( S_DOOR_SPOELEN );
          ledBlink();  //led blink
        }

        if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)   &&   knop == KNOP_TERUGSPOEL){//      << TERUG SPOELEN
          setStaat( S_TERUG_SPOELEN );
          ledBlink();  //led blink
        }

        



        if(knop == KNOP_PLAY){
          if(staat == S_BEGINNEN_SPELEN || staat == S_NAAR_BEGIN_PLAAT || staat == S_PLAAT_DIAMETER_METEN || staat == S_NAALD_EROP){//         STOPPEN
            stoppen();
            ledBlink();  //                  led blink
          }else{                                 
            spelen();//                       SPELEN
            ledBlink();  //                  led blink
          }
        }
      }
      
      
      
      
      else if( knopStaat[knop] == LANG_INGEDRUKT  &&    knopIn[knop] == LOSGELATEN ){//                          LANG LOSGELATEN
        knopStaat[knop] = LOSGELATEN;
        knopAlleInterval = millis();
        
        // Serial.print(knopNaam( knop));Serial.println(" lang los ");



        if(  (  staat == S_DOOR_SPOELEN   ||   staat == S_TERUG_SPOELEN  )  &&  (  knop == KNOP_DOORSPOEL  ||  knop == KNOP_TERUGSPOEL  )  ){//WEER BEGINNEN NA SPOELEN
          naaldErop();
          ledBlink();  //led blink
          
        }
      }
      
      
      
      
      else if( knopStaat[knop] == LANG_INGEDRUKT    &&    millis() - knopInterval[knop]  >  KNOP_SUPER_LANG ){//              SUPER LANG INGEDRUKT
        knopStaat[knop] = SUPER_LANG_INGEDRUKT;
        knopAlleInterval = millis();   //led blink
        
        // Serial.print(knopNaam( knop));Serial.println(" super lang ");


        if(  staat == S_HOK  &&  knop == KNOP_TERUGSPOEL  ){//               NAALD TEST STAND
          setStaat( S_BEGINNEN_SCHOONMAAK);
          ledBlink();  //led blink
        }
      }
      
      
      
      
      else if( knopStaat[knop] == SUPER_LANG_INGEDRUKT  &&    knopIn[knop] == LOSGELATEN ){//                    SUPER LANG LOSGELATEN
        knopStaat[knop] = LOSGELATEN;
        knopAlleInterval = millis();   //led blink
        
        // Serial.print(knopNaam( knop));Serial.println(" super lang los ");

        
        if(  (  staat == S_DOOR_SPOELEN   ||   staat == S_TERUG_SPOELEN  )  &&  (  knop == KNOP_DOORSPOEL  ||  knop == KNOP_TERUGSPOEL  )  ){//WEER BEGINNEN NA SPOELEN
          naaldErop();
          ledBlink();  //led blink
        }
      }
      
      
      
      
      else if( knopIn[knop] == LOSGELATEN ){ //nog ff checken of de knop is losgelaten zodat en lange druk geen loop maakt
        knopStaat[knop] = LOSGELATEN;

      
      }
    
    }









    potVal = analogRead(displayPOTMETERanaloog);

    if(!isOngeveer(potVal, potValPrev, AMAX/2)){
      if(potVal > potValPrev){
        potValPrev += AMAX;
      }else{
        potValPrev -= AMAX;        
      }
    }

    potVolume    +=     float(potVal - potValPrev) / AMAX;
    potVolumeFilter += (potVolume - potVolumeFilter)/10;
    potVolumeDiv = potVolumeFilter - potVolumeFilterPrev;
    potVolumeFilterPrev = potVolumeFilter;

    if(staat == S_PAUZE){
      // if(!isOngeveer(potVolumeDiv , 0, 0.0005)){
        karTargetPos -= potVolumeDiv * 50;        
      // }
    }else{

    }
    
    potValPrev = potVal;


  }
}

























