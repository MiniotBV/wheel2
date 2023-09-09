#define KNOP_PLAY 6//~5  knop3
#define KNOP_DOORSPOEL 5//~4  knop2
#define KNOP_TERUGSPOEL 7//~6   knop4


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

Interval ledBlinkInterval(0, MILLIS);
Interval knopAlleInterval(0, MILLIS);
Interval volumeDisplayActie(0, MILLIS);
Interval rpmDisplayActie(0, MILLIS);


int potVal = 0;
int potValPrev = 0;

float riem = 0;
float riemPrev = 0;
float riemFilter;
float riemFilterPrev;

float riemFilterDiv;
float riemDiv;







void printKnoppen(){
  Serial.print("knoppen: ");
  for(int i = 0; i < 8; i++){
    Serial.print(knopIn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // Serial.print("staat:   ");
  // for(int i = 0; i < 8; i++){
  //   Serial.print(knopStaat[i]);
  //   Serial.print(' ');
  // }
  // Serial.println();


}



void ledBlink(){
  ledBlinkInterval.reset();
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
  // if(knop == KNOP_BT){
  //   return "bt";
  // }
  // if(knop == KNOP_RPM){
  //   return "rpm";
  // }

  return "?";
}



bool knopDebug = false;

void knopLog(int knop, const char* actie){
  if(knopDebug){
    Serial.print(knopNaam(knop));
    Serial.println(actie);
  }
}




void getKnopData(){
    gpio_put(displayLATCH, 0);
    gpio_put(displayLATCH, 1);

    for(int i = 0; i < 8; i++){
    
      knopIn[i] = digitalRead(displayUIT);

      gpio_put(displayKLOK, 1);
      gpio_put(displayKLOK, 0);
    }
}






int gecompenseerdeDoorspoel(){
  if(orientatie.isStaand){
    return KNOP_TERUGSPOEL;
  }
  return KNOP_DOORSPOEL;  
}

int gecompenseerdeTerugspoel(){
  if(orientatie.isStaand){
    return KNOP_DOORSPOEL;
  }
  return KNOP_TERUGSPOEL;
}

bool isKnopDoorspoel(int knop){
  return knop == gecompenseerdeDoorspoel();
}

bool isKnopTerugspoel(int knop){
  return knop == gecompenseerdeTerugspoel();
}








Interval knoppenInt(10000, MICROS);

void knoppenUpdate(){
  if(knoppenInt.loop()){

    getKnopData();


    for(int knop = 5; knop < 8; knop++){

      
      if(       knopStaat[knop] == LOSGELATEN   &&    knopIn[knop] == INGEDRUKT ){//             KORT INGEDRUKT
        knopStaat[knop] = INGEDRUKT;
        
        knopAlleInterval.reset();
        knopInterval[knop] = millis();

        ledBlink();  //led blink
        
        knopLog( knop, " in ");
        
        if( staat == S_SCHOONMAAK ){//   SCHOONMAAK STAND STOPPEN
          stoppen();
          // ledBlink();  //led blink
        }
        

      }
      
      

      
      else if( knopStaat[knop] == INGEDRUKT    &&    knopIn[knop] == LOSGELATEN ){//               KORT LOSGELATEN
        knopStaat[knop] = LOSGELATEN;
        knopAlleInterval.reset();
        
        knopLog( knop, " los ");
        
        if(isKnopDoorspoel(knop)){
          
          if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)){
            naarVolgendNummer();
          }
        }


        if(isKnopTerugspoel(knop)){

          if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)){
            naarVorrigNummer();
          }
        }

        
        if(knop == KNOP_PLAY){
          if(staat == S_PAUZE  ||  staat == S_NAALD_EROP){
            pauze();
          


          }else if(staat == S_HOK){
            spelen();
          }
        }


        if(knop == KNOP_TERUGSPOEL){
          if(staat == S_HOK){
            if(rpmStaat == AUTO){
              rpmStaat = R33;
            }
            else if(rpmStaat == R33){
              rpmStaat = R45;
            }
            else{
              rpmStaat = AUTO;
            }

            updatePlateauRpm();
            rpmDisplayActie.reset();
          }
        }



      }
      
      
      
      
      else if( knopStaat[knop] == INGEDRUKT    &&    millis() - knopInterval[knop]  >  KNOP_LANG ){//               LANG INGEDRUKT
        knopStaat[knop] = LANG_INGEDRUKT;
        knopAlleInterval.reset();

        knopLog( knop," lang ");


        
        
        if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)  &&   isKnopDoorspoel(knop)){ //      >> DOOR SPOELEN
          setStaat( S_DOOR_SPOELEN );
        }

        if( (staat == S_NAALD_EROP  ||  staat == S_PAUZE  ||  staat == S_NAAR_NUMMER)   &&   isKnopTerugspoel(knop)){//      << TERUG SPOELEN
          setStaat( S_TERUG_SPOELEN );
        }

        



        if(knop == KNOP_PLAY){
          if(staat == S_HOK){//         SPELEN
            spelen();//                       SPELEN
          }else{                                 
            stoppen();
          }
        }
      }
      
      
      
      
      else if( knopStaat[knop] == LANG_INGEDRUKT  &&    knopIn[knop] == LOSGELATEN ){//                          LANG LOSGELATEN
        knopStaat[knop] = LOSGELATEN;
        knopAlleInterval.reset();
        
        knopLog(  knop," lang los ");



        if(  (  staat == S_DOOR_SPOELEN   ||   staat == S_TERUG_SPOELEN  )  &&  (  knop == KNOP_DOORSPOEL  ||  knop == KNOP_TERUGSPOEL  )  ){//WEER BEGINNEN NA SPOELEN
          setStaat(S_NAALD_EROP);
          // ledBlink();  //led blink
          
        }
      }
      
      
      
      
      else if( knopStaat[knop] == LANG_INGEDRUKT    &&    millis() - knopInterval[knop]  >  KNOP_SUPER_LANG ){//              SUPER LANG INGEDRUKT
        knopStaat[knop] = SUPER_LANG_INGEDRUKT;
        knopAlleInterval.reset();   //led blink
        
        knopLog(  knop, " super lang ");


        if(  staat == S_HOK  &&  knop == KNOP_TERUGSPOEL  ){//               NAALD TEST STAND
          setStaat( S_SCHOONMAAK );
        }
      }
      
      
      
      
      else if( knopStaat[knop] == SUPER_LANG_INGEDRUKT  &&    knopIn[knop] == LOSGELATEN ){//                    SUPER LANG LOSGELATEN
        knopStaat[knop] = LOSGELATEN;
        knopAlleInterval.reset();  //led blink
        
        knopLog(  knop, " super lang los ");
        
        
        if(  (  staat == S_DOOR_SPOELEN   ||   staat == S_TERUG_SPOELEN  )  &&  (  knop == KNOP_DOORSPOEL  ||  knop == KNOP_TERUGSPOEL  )  ){//WEER BEGINNEN NA SPOELEN
          setStaat(S_NAALD_EROP);
        }
      }
      
      
      
      
      else if( knopIn[knop] == LOSGELATEN ){ //nog ff checken of de knop is losgelaten zodat en lange druk geen loop maakt
        knopStaat[knop] = LOSGELATEN;

      
      }
    
    }














    potVal = analogRead(displayPOTMETER);

    if(!isOngeveer(potVal, potValPrev, AMAX/2)){
      if(potVal > potValPrev){
        potValPrev += AMAX;
      }else{
        potValPrev -= AMAX;        
      }
    }

    riem    +=     float(potVal - potValPrev) / AMAX;
    potValPrev = potVal;
    
    riemFilter += (riem - riemFilter)/3;


    


    if( !isOngeveer(riemFilter, riemFilterPrev, 0.01) ){
      riemFilterDiv = riemFilter - riemFilterPrev;
      riemFilterPrev = riemFilter;
      
      riemDiv = riemFilterDiv;    
      // riemDiv = riem - riemPrev;
      riemDiv = -riemDiv; // flip

      if(staat == S_SCHOONMAAK){
        armGewicht += riemDiv * 4;
        armGewichtUpdate();
      }

      else if(staat == S_PAUZE){
        targetNummerPos -= riemDiv * 50;
        targetNummerPos = limieteerF(targetNummerPos, PLAAT_EINDE, plaatBegin);      
      }
    
      else{
        if(staat != S_DOOR_SPOELEN   &&  staat != S_TERUG_SPOELEN   &&  staat != S_NAAR_NUMMER   &&  staat != S_PAUZE){ //zodat niet volume oppopt na een knop indruk met een doorspoel ofzo
          volumeDisplayActie.reset();
        }
        
        // volume = limieteerF(volume + (riemDiv * stappenPerOmwenteling), 0, 63);
        volume += round(riemDiv * 100);
        volume = limieteerI(volume, 0, 63);        
      }
    }
    






  }
}

























