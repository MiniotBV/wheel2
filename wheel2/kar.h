//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen
// #include "plaatLees.h"

bool plaatAanwezig = false;

float karP = 0.1; //0.00005;//0.00025;
float karI = 0.005; //0.00005;//0.00025;
float karPcomp = 0;




// #define PLAAT_BEGIN 148
#define PLAAT_BEGIN 147
// #define PLAAT_EINDE 52.5
#define PLAAT_EINDE 55

#define KAR_HOME 43.5
#define KAR_HOK 45.5

#define SCHOONMAAK_PLEK 100

#define SENSOR_OFFSET 7.5//mm

// float mmPerStap = 1.5 / ( 48 / 12 );
float mmPerStap = 1.5 / ( 48 / 8 );
float stap2mm = ( 2 / PI ) * mmPerStap;  // 0.238732414637843
float mm2stap = 1 / stap2mm;             // 4.188790204786391




bool karMotorEnable = true;
float karMotorPos = 0;

float karPos = KAR_HOK;
float targetNummerPos = 0;
float sensorPos;
float karOffset = 0; // om te kijken wat de homing afwijking is

float plaatAanwezigSindsKarPos = 0;


float armHoekRuw = analogRead(hoekSensor);
float armHoek;
float armHoekFilterWaarde = 1;
float armHoekSlow = armHoekRuw;
float armHoekSlowFilterWaarde = 100;
float armHoekOffset = 1920;




void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);
}



#define KAR_MAX_SNELHEID 0.02
#define KAR_VERSNELLING  0.00004

double karSnelHeid = 0;
// float karTargetPos = 0;
float versnelling = 0;



bool beweegKarNaarPos(float target, float snelheid){

  versnelling = 0;
  
  float afstandTeGaan = abs(target - karPos);
  
  int richting =  target - karPos > 0 ? 1 : -1;

  float afstandOmTeStoppen = (karSnelHeid * karSnelHeid) / ( 2 * KAR_VERSNELLING );


  if(isOngeveer(afstandTeGaan,  0,  0.01)  &&  afstandOmTeStoppen < 0.1){ // moet nog ff checken of 1mm goed is waarschijnlek niet
    karSnelHeid = 0;
    return true;
  }

  if(afstandOmTeStoppen >= afstandTeGaan){
    versnelling = -KAR_VERSNELLING * richting;
  }
  else if(abs(karSnelHeid) < KAR_MAX_SNELHEID){
    versnelling = KAR_VERSNELLING * richting;
  }

  
  karPos += karSnelHeid + (versnelling / 2);

  karSnelHeid += versnelling;



  karSnelHeid = limieteerF(karSnelHeid, -snelheid, snelheid);

  return false;
}


//accel         0   2   2   2   0   0   0   -2  -2  -2
//speed         0   2   4   6   6   6   6   4   2   0
//pos           0   1   4   9   15  21  27  32  35  36

//accel     0   2   2   2   2   0   0   0   -2  -2  -2  -2
//speed     0   2   4   6   8   8   8   8   6   4   2   0
//pos       0   1   4   9   16  24  32  40  47  52  55  56












void armHoekCalibreer(){
  armHoekOffset = armHoekSlow;
  // Serial.print("armHoekofset: ");
  // Serial.println(armHoekOffset);
}




void gaNaarNummer(float pos){
  targetNummerPos = pos;
  Serial.print("naarpos:");
  Serial.println(targetNummerPos);
  setStaat(S_NAAR_NUMMER);
}








void naarVorrigNummer(){
  if(hoeveelNummers < 2){// als er 1 nummer is is dat ook te weinig
    gaNaarNummer(PLAAT_BEGIN);
    return;
  }

  float pos = karPos;

  if(staat == S_NAAR_NUMMER){//als er al word door gespoeld doe dan een extra nummer verder
    pos = targetNummerPos;
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
    pos = targetNummerPos;
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
    targetNummerPos = karPos;
  }
  else if(staat == S_PAUZE){
    setStaat(S_NAALD_EROP);
  }
}






















void staatDingen(){
  
  if(staat == S_STOPPEN){
    // if(millis() < 3000){//pas 1 seconden na opstart naar home gaan om de armhoek sensor te calibreren
    //   armHoekCalibreer();
    //   return;
    // }
    if(naaldEraf()){
      // karMotorEnable = true;
      setStaat(S_NAAR_HOK);
    }
    return;
  }



  if(staat == S_NAAR_HOK  ||  staat == S_HOMEN_VOOR_SPELEN){
    if(staatVeranderd.sinds() < 400){//even wachten en teruch rijden
      karPos += KAR_MAX_SNELHEID/5;
      return;
    }

    // if(staatVeranderd.sinds() < 1000){//calibreren
    //   armHoekCalibreer();
    //   return;
    // }

    if(armHoek < -800){//-1000){
      Serial.print("div; ");
      Serial.println(  (karOffset + karPos)  );
      karOffset -= KAR_HOME - karPos;
      karPos = KAR_HOME;

      if(staat == S_HOMEN_VOOR_SPELEN){
        setStaat(S_BEGINNEN_SPELEN);
      }else{
        setStaat(S_HOK);
      }

      return;
    }
    
    if(armHoek < -200){
      karPos -= KAR_MAX_SNELHEID/10;
      return;
    }
    
    karPos -= KAR_MAX_SNELHEID;
    return;
  }



  if(staat == S_HOK){
    if(!beweegKarNaarPos(KAR_HOK, KAR_MAX_SNELHEID)){
      return;
    } 
    
    armHoekCalibreer();
    karMotorEnable = false;
    return;
  }



  if(staat == S_FOUTE_ORIENTATIE){
    naaldEraf();
    karMotorEnable = false;
    return;
  }



  if(staat == S_BEGINNEN_SPELEN){
    
    // if(karPos  <  PLAAT_EINDE + SENSOR_OFFSET){
    //   karPos += KAR_MAX_SNELHEID;
    if(  !beweegKarNaarPos(PLAAT_EINDE + SENSOR_OFFSET,   KAR_MAX_SNELHEID)  ){ return;}
    
    setStaat(S_PLAAT_AANWEZIG);
    return;
  }



  if(staat == S_NAAR_BEGIN_PLAAT){
    if(  !beweegKarNaarPos(PLAAT_BEGIN,   KAR_MAX_SNELHEID)  ){ return;}

    setStaat(S_PLAAT_DIAMETER_METEN);
    Serial.print("nummers gevonden: ");
    Serial.println(hoeveelNummers);
    
    return;
  }



  if(staat == S_PLAAT_DIAMETER_METEN){
    if(staatVeranderd.sinds() > 500){ // even een halve seconden wachten om de plaat detectie te laten werken
      
      if(plaatAanwezig  &&  plaatAanwezigSindsKarPos == 0){
        
        if(isOngeveer(karPos, PLAAT_BEGIN, 1)){ // is het grofweg 12inch dan is het een gewone plaat en kan ie geweoon weer spelen
          setStaat(S_NAALD_EROP);
          Serial.println("plaatDia: 12inch");
          return;
        
        }
        
        // anders moet er gekeken worden wat de maat is en welke rpm daar bijhoort
        plaatAanwezigSindsKarPos = karPos - SENSOR_OFFSET;
        float inchDia = (plaatAanwezigSindsKarPos / 25.4)*2;
        if(inchDia < 8){//isOngeveer(inchDia, 7, 1)){
          Serial.print("±7\" ");
          setPlateauRpm(45);
        }
        Serial.print("plaatDia: ");
        Serial.println(inchDia);
        
        return;
      }
      
      
      if(karPos >  plaatAanwezigSindsKarPos + 0){
        
        karPos -= KAR_MAX_SNELHEID;
        if(karPos <= SENSOR_OFFSET + PLAAT_EINDE){
          stoppen();
        }
        return;
      }
      
      setStaat(S_NAALD_EROP);

      return;
    
    
    }
    
    plaatAanwezigSindsKarPos = 0;
    return;
  }



  if(staat == S_NAALD_EROP){
    if(naaldErop()){
      // if(abs(armHoek) > 10){
        // karPcomp = limieteerF( -karP * armHoek , -KAR_MAX_SNELHEID / 2, KAR_MAX_SNELHEID / 2);
        karPcomp += ( limieteerF( armHoek * -karP,     -3, 3) - karPcomp ) / 500;
        karPos += karPcomp * karI;
        karPos = limieteerF( karPos, PLAAT_EINDE, PLAAT_BEGIN + 2);
      // }
      
      if(karPos <= PLAAT_EINDE){
        stoppen();
      }
    }
    return;
  }else{
    karPcomp = 0;
  }



  if(staat == S_NAAR_NUMMER){
    if(naaldEraf()){
      if(beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID)){
      // if(karPos == targetNummerPos){
        setStaat(S_NAALD_EROP);
        return;
      }
      
      // karPos += limieteerF( targetNummerPos - karPos , -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);
    }
    return;
  }




  else if(staat == S_JOGGEN){
    if(naaldEraf()){
      karPos += limieteerF( targetNummerPos - karPos , -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);
    }
  }



  if(staat == S_PAUZE){
    if(naaldEraf()){
      if(staatVeranderd.sinds() > 3000){
        armHoekCalibreer();
      }
      
      targetNummerPos = limieteerF(targetNummerPos, PLAAT_EINDE, PLAAT_BEGIN);
      beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID);
      
      // karPos += limieteerF( (targetNummerPos - karPos) / 10 , -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);
      // karPos = limieteerF( karPos, PLAAT_EINDE, PLAAT_BEGIN);
      
    }
    return;
  }




  if(staat == S_SCHOONMAAK){
    if(beweegKarNaarPos( SCHOONMAAK_PLEK,   KAR_MAX_SNELHEID)){  naaldErop(); }
    return;
  }

}




































Interval armHoekInt(100, MICROS);
Interval karMotorInt(1000, MICROS);


void karMotorFunc(){
  // if(armHoekInt.loop()){

  //   // armHoek = analogRead(hoekSensor);
  //   armHoekRuw += (analogRead(hoekSensor) - armHoekRuw) / armHoekFilterWaarde;
  //   armHoekSlow += (armHoekRuw - armHoekSlow) / armHoekSlowFilterWaarde;

  //   armHoek = armHoekRuw - armHoekOffset;
  // }



    
      
  if(karMotorInt.loop()){

    // armHoekRuw += (analogRead(hoekSensor) - armHoekRuw) / armHoekFilterWaarde;
    armHoekRuw = analogRead(hoekSensor);
    armHoekSlow += (armHoekRuw - armHoekSlow) / armHoekSlowFilterWaarde;

    armHoek = armHoekRuw - armHoekOffset;


    sensorPos = karPos - SENSOR_OFFSET;

    

    staatDingen();



    karMotorPos = (karPos + karOffset + karPcomp)  *  mm2stap;
    



    if(karMotorEnable){
    // if(staat != S_HOK){

      pwmFase( sin(karMotorPos),  stapperAP, stapperAN, true);
      pwmFase( cos(karMotorPos),  stapperBP, stapperBN, true);
    
    }else{

      pwmFase( 0,  stapperAP, stapperAN, true);
      pwmFase( 0,  stapperBP, stapperBN, true);
    }

    karMotorEnable = true;

  }
}













