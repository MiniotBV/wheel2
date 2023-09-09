//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen
// #include "plaatLees.h"

bool plaatAanwezig = false;

#define KAR_SNELHEID 0.02

#define PLAAT_BEGIN 148
#define PLAAT_EINDE 52.5

#define KAR_HOME 44.5
#define KAR_HOK 45.5

#define SCHOONMAAK_PLEK 100

#define SENSOR_NAALT_OFFSET 7.5//mm

// float mmPerStap = 1.5 / ( 48 / 12 );
float mmPerStap = 1.5 / ( 48 / 8 );
float stap2mm = ( 2 / PI ) * mmPerStap;  // 0.238732414637843
float mm2stap = 1 / stap2mm;             // 4.188790204786391

float karP = 0.0001;//0.00025;

int stapperFaseA = 0;
int stapperFaseB = 0;

bool karMotorEnable = true;
float karMotorPos = 0;
float karOffset = 0;
float karPos = 0;
float karTargetPos = 0;
float sensorPos;

float plaatAanwezigSindsKarPos = 0;


float armHoekRuw = analogRead(hoekSensor);
float armHoek;
float armHoekFilterWaarde = 1;
float armHoekSlow = armHoek;
float armHoekSlowFilterWaarde = 1000;
float armHoekOffset = 1920;




void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);
}


void armHoekCalibreer(){
  armHoekOffset = armHoekSlow;
  Serial.print("armHoekofset: ");
  Serial.println(armHoekOffset);
}




void gaNaarNummer(float pos){
  karTargetPos = pos;
  Serial.print("naarpos:");
  Serial.println(karTargetPos);
  setStaat(S_NAAR_NUMMER);
}








void naarVorrigNummer(){
  if(hoeveelNummers < 2){// als er 1 nummer is is dat ook te weinig
    gaNaarNummer(PLAAT_BEGIN);
    return;
  }

  float pos = karPos;

  if(staat == S_NAAR_NUMMER){//als er al word door gespoeld doe dan een extra nummer verder
    pos = karTargetPos;
  }

  int nummer = 0;

  while(pos + 2 >= nummers[nummer]){
    nummer++;
    if(nummer > hoeveelNummers - 1){
      gaNaarNummer(PLAAT_BEGIN);
      return;
    }
  }

  gaNaarNummer(nummers[nummer]);
}






void naarVolgendNummer(){
  if(hoeveelNummers < 2){ // als er 1 nummer is is dat ook te weinig
    stoppen();
    return;
  }

  float pos = karPos;
  
  if(staat == S_NAAR_NUMMER){//als er al word door gespoeld doe dan een extra nummer verder
    pos = karTargetPos;
  }

  int nummer = hoeveelNummers - 1;

  while(pos - 2 <= nummers[nummer]){
    nummer--;
    if(nummer <= 0){ // nummer 0 is het begin van nummers (dus niet en egt nummer)
      stoppen();
      return;
    }
  }

  gaNaarNummer(nummers[nummer]);
}









void pauze(){
  if(staat == S_NAALD_EROP){
    setStaat(S_PAUZE);
    karTargetPos = karPos;
  }
  else if(staat == S_PAUZE){
    setStaat(S_NAALD_EROP);
  }
}


















Interval armHoekInt(100, MICROS);
Interval karMotorInt(1000, MICROS);


void karMotorFunc(){
  if(armHoekInt.loop()){

    // armHoek = analogRead(hoekSensor);
    armHoekRuw += (analogRead(hoekSensor) - armHoekRuw) / armHoekFilterWaarde;
    armHoekSlow += (armHoekRuw - armHoekSlow) / armHoekSlowFilterWaarde;

    armHoek = armHoekRuw - armHoekOffset;
  }



    
      
  if(karMotorInt.loop()){

    if(staat == S_STOPPEN){
      if(isNaaldEraf()  &&  millis() > 1000){//pas 1 seconden na opstart naar home gaan om de armhoek sensor te calibreren
        armHoekCalibreer();
        setStaat(S_NAAR_HOK);
      }
    }

    else if(staat == S_NAAR_HOK  ||  staat == S_HOMEN_VOOR_SPELEN){
      if(armHoek < -600){
        Serial.print("div; ");
        Serial.println(  (karOffset + karPos)  );
        karOffset -= KAR_HOME - karPos;
        karPos = KAR_HOME;

        if(staat == S_HOMEN_VOOR_SPELEN){
          setStaat(S_BEGINNEN_SPELEN);
        }else{
          setStaat(S_HOK);
        }
        
      }else{
        if(armHoek < -300){
          karPos -= KAR_SNELHEID/10;
        }else{
          karPos -= KAR_SNELHEID;
        }
      }
    }



    else if(staat == S_HOK){
      if(karPos < KAR_HOK){
        karPos += KAR_SNELHEID;
      }
    }



    else if(staat == S_BEGINNEN_SPELEN){
      if(karPos  <  PLAAT_EINDE + SENSOR_NAALT_OFFSET){
        karPos += KAR_SNELHEID;
      }else{
        setStaat(S_PLAAT_AANWEZIG);
      }

    }



    else if(staat == S_NAAR_BEGIN_PLAAT){
      if(karPos < PLAAT_BEGIN){
        karPos += KAR_SNELHEID;
      }else{
        setStaat(S_PLAAT_DIAMETER_METEN);
        Serial.print("nummers gevonden: ");
        Serial.println(hoeveelNummers);
      }
    }



    else if(staat == S_PLAAT_DIAMETER_METEN){
      if(staatVeranderd.sinds() > 500){
        if(plaatAanwezig){
          if(isOngeveer(karPos, PLAAT_BEGIN, 1)){
            naaldErop();
            Serial.println("plaatDia: 12inch");
          
          }else if(plaatAanwezigSindsKarPos == 0){
            plaatAanwezigSindsKarPos = karPos;
            Serial.print("plaatDia: ");
            Serial.println((karPos / 25.4)*2);
          
          }else if(plaatAanwezigSindsKarPos - karPos <  SENSOR_NAALT_OFFSET + 0){
            karPos -= KAR_SNELHEID;
          
          }else{
            naaldErop();
            
          }
        }else{
          plaatAanwezigSindsKarPos = 0;
          karPos -= KAR_SNELHEID;
        }
      }
    }



    else if(staat == S_NAALD_EROP){
      if(isNaaldErop()){
        karPos += limieteerF( -karP * armHoek , -KAR_SNELHEID / 2, KAR_SNELHEID / 2);
        karPos = limieteerF( karPos, PLAAT_EINDE, PLAAT_BEGIN + 2);
        if(karPos <= PLAAT_EINDE){
          stoppen();
        }
      }

    }



    else if(staat == S_NAAR_NUMMER){
      if(isNaaldEraf()){
        if(karPos == karTargetPos){
          naaldErop();
        }else{
          karPos += limieteerF( karTargetPos - karPos , -KAR_SNELHEID, KAR_SNELHEID);
        }
      }
    }



    else if(staat == S_PAUZE){
      if(isNaaldEraf()){
        if(karPos == karTargetPos){
          // naaldErop();
        }else{
          karPos += limieteerF( (karTargetPos - karPos) / 10 , -KAR_SNELHEID, KAR_SNELHEID);
          karPos = limieteerF( karPos, PLAAT_EINDE, PLAAT_BEGIN);
        }
      }
    }



    else if(staat == S_DOOR_SPOELEN){
      if(isNaaldEraf()){
        if(karPos >= PLAAT_EINDE){
          karPos -= KAR_SNELHEID/2;
        }
      }
    }



    else if(staat == S_TERUG_SPOELEN){
      if(isNaaldEraf()){
        if(karPos <= PLAAT_BEGIN){
          karPos += KAR_SNELHEID/2;
        }
      }
    }








    else if(staat == S_SCHOONMAAK){
      if(karPos < SCHOONMAAK_PLEK){
        karPos += KAR_SNELHEID;
      }else{
        naaldErop();
      }
    }





    sensorPos = karPos - SENSOR_NAALT_OFFSET;
    karMotorPos = (karPos + karOffset)  *  mm2stap;


    if(karMotorEnable){

      pwmFase( sin(karMotorPos),  stapperAP, stapperAN, true);
      pwmFase( cos(karMotorPos),  stapperBP, stapperBN, true);
    
    }else{

      pwmFase( 0,  stapperAP, stapperAN, true);
      pwmFase( 0,  stapperBP, stapperBN, true);
    }

  }
}













