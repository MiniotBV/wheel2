//motor heeft 48 stappen
//tandwiel heeft 12 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap

//PI = 2 stappen
// #include "plaatLees.h"


float karInterval;


bool plaatAanwezig = false;

float karP = 0.1; //0.00005;//0.00025;
float karI = 0.005; //0.00005;//0.00025;
float karPcomp = 0;




// #define GROOTSTE_PLAAT_BEGIN 148
#define GROOTSTE_PLAAT_BEGIN 147
// #define PLAAT_EINDE 52.5
#define PLAAT_EINDE 52.5

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

float plaatBegin = 0;


float armHoekRuw = analogRead(hoekSensor);
float armHoek;
float armHoekFilterWaarde = 1;
float armHoekSlow = armHoekRuw;
float armHoekSlowFilterWaarde = 100;
float armHoekOffset = 1920;









#define KAR_MAX_SNELHEID 0.02
#define KAR_VERSNELLING  0.0001

double karSnelHeid = 0;
// float karTargetPos = 0;
float versnelling = 0;
float afstandOmTeStoppen = 0;



bool beweegKarNaarPos(float target, float snelheid){

  versnelling = 0;
  
  float teGaan = abs(target - karPos);
  int teGaanRichting =  target - karPos > 0 ? 1 : -1;



  afstandOmTeStoppen = (karSnelHeid * karSnelHeid) / ( 2 * KAR_VERSNELLING );
  int afstandOmTeStoppenRichting = karSnelHeid > 0 ? 1 : -1;

  if(isOngeveer(teGaan,  0,  0.01)  &&  afstandOmTeStoppen < 0.1){
    karSnelHeid = 0;
    return true;
  }

  if(afstandOmTeStoppen >= teGaan){
    versnelling = -KAR_VERSNELLING * teGaanRichting;
  }
  else if(abs(karSnelHeid) < KAR_MAX_SNELHEID){
    versnelling = KAR_VERSNELLING * teGaanRichting;
  }

  
  karPos += karSnelHeid + (versnelling / 2);

  karSnelHeid += versnelling;



  karSnelHeid = limieteerF(karSnelHeid, -snelheid, snelheid);

  return false;
}



void karNoodStop(){
  karSnelHeid = 0;
}








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
    gaNaarNummer(plaatBegin);
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
      gaNaarNummer(plaatBegin);
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










bool isPlaatOngeveer7Inch(){
  float inchDia = (plaatBegin / 25.4)*2;
  return inchDia < 8;
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
    if(naaldEraf()){
      setStaat(S_NAAR_HOK);
    }
    return;
  }



  if(staat == S_NAAR_HOK  ||  staat == S_HOMEN_VOOR_SPELEN){
    if(staatVeranderd.sinds() < 2000){//calibreren
      armHoekCalibreer();
      return;
    }

    bool aangekomen = false;

    // if(armHoek < -200){
    //   aangekomen = beweegKarNaarPos(-150,  KAR_MAX_SNELHEID/10);
    // }else{
      aangekomen = beweegKarNaarPos(-150,  KAR_MAX_SNELHEID);
    // }

    if(aangekomen){
      karOffset -= KAR_HOME - karPos;
      karPos = KAR_HOME;
      setStaat(S_HOMEN_GEFAALD);
      return;
    }

    if(armHoek < -800){//-1000)
      Serial.print("div; ");
      Serial.println(  (karOffset + karPos)  );
      karOffset -= KAR_HOME - karPos;
      karPos = KAR_HOME;

      karNoodStop();

      if(staat == S_HOMEN_VOOR_SPELEN){
        setStaat(S_BEGINNEN_SPELEN);
      }else{
        setStaat(S_HOK);
      }

      return;
    }
    
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



  if(staat == S_HOMEN_GEFAALD){
    if(beweegKarNaarPos(KAR_HOK+10, KAR_MAX_SNELHEID)){
      setStaat(S_NAAR_HOK);
    } 
    return;
  }



  if(staat == S_FOUTE_ORIENTATIE){
    naaldEraf();
    karMotorEnable = false;
    return;
  }





  if(staat == S_BEGINNEN_SPELEN){
    if( beweegKarNaarPos(PLAAT_EINDE + SENSOR_OFFSET,   KAR_MAX_SNELHEID)  ){ 
      setStaat(S_PLAAT_AANWEZIG);
    }
    return;
  }





  if(staat == S_NAAR_BEGIN_PLAAT){
    if(  beweegKarNaarPos(GROOTSTE_PLAAT_BEGIN,   KAR_MAX_SNELHEID)  ){
      setStaat(S_PLAAT_DIAMETER_METEN);
      Serial.print("nummers gevonden: ");
      Serial.println(hoeveelNummers);
    }
    
    return;
  }




  if(staat == S_PLAAT_DIAMETER_METEN){
    if(staatVeranderd.sinds() < 500){ // even een halve seconden wachten om de plaat detectie te laten werken
      plaatBegin = 0;
      return;
    }

    if(plaatAanwezig  ||  plaatBegin != 0){ // is er een plaat aanwezig
      
      if(plaatBegin == 0){    // voer maar 1 keer uit
        if(isOngeveer(karPos, GROOTSTE_PLAAT_BEGIN, 1)){
          plaatBegin = GROOTSTE_PLAAT_BEGIN;
          Serial.println("plaatDia: 12inch");
          setPlateauRpm(rpm33);
        
        }else{
          // anders moet er gekeken worden wat de maat is en welke rpm daar bijhoort
          plaatBegin = karPos - SENSOR_OFFSET;

          if(isPlaatOngeveer7Inch()){//isOngeveer(inchDia, 7, 1)){
            Serial.print("±7\" ");
            setPlateauRpm(rpm45);
          }else{
            setPlateauRpm(rpm33);
          }
          Serial.println("plaatDia: " + String((plaatBegin / 25.4)*2) + "\"");
          
        }
      }
      
      if(beweegKarNaarPos(plaatBegin, KAR_MAX_SNELHEID)){ 
        setStaat(S_NAALD_EROP);
        return;
      }

      return;
    }

    if( beweegKarNaarPos(SENSOR_OFFSET + PLAAT_EINDE, KAR_MAX_SNELHEID) ){ 
      stoppen();
      return;
    }

    return;
  }




  if(staat == S_NAALD_EROP){
    if(naaldErop()){
      karPcomp += ( limieteerF( armHoek * -karP,     -3, 3) - karPcomp ) / 500;
      karPos += karPcomp * karI;
      karPos = limieteerF( karPos, PLAAT_EINDE, plaatBegin + 1);
      
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




  if(staat == S_DOOR_SPOELEN){
    if(naaldEraf()){
    }
  }

  if(staat == S_TERUG_SPOELEN){
    if(naaldEraf()){
    }
  }




  if(staat == S_PAUZE){
    if(naaldEraf()){
      if(staatVeranderd.sinds() > 3000){
        armHoekCalibreer();
      }
      
      // targetNummerPos = limieteerF(targetNummerPos, PLAAT_EINDE, plaatBegin);
      beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID);
      
      // karPos += limieteerF( (targetNummerPos - karPos) / 10 , -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);
      // karPos = limieteerF( karPos, PLAAT_EINDE, GROOTSTE_PLAAT_BEGIN);
      
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



bool karMotorUitvoeren(){
  
  karInterval = (micros() - karMotorInt.vorrigeVorrigeTijd) / 1000000.0;

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

  return 1;
}



void karMotorFunc(){
      
  if(karMotorInt.loop()){
    karMotorUitvoeren();
  }
}





void karInit(){
  setPwm(stapperAP);
  setPwm(stapperAN);
  setPwm(stapperBP);
  setPwm(stapperBN);

  // struct repeating_timer timer;
  // add_repeating_timer_us(1, karMotorUitvoeren, NULL, &timer);
  // add_repeating_timer_us(1000, karMotorUitvoeren, NULL, &karTimer);
}






