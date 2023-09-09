//motor heeft 48 stappen
//tandwiel heeft 8 tanden
//tandheugel heeft 1.5mm tand pitch 
//48 stappen / 12 tanden = 4 stappen per tand
//1.5mm / 4 stappen per tand = 0.375mm per stap
//PI = 2 stappen
float mmPerStap = 1.5 / ( 48 / 8 );// / 12 );
float stap2mm = ( 2 / PI ) * mmPerStap;  // 0.238732414637843
float mm2stap = 1 / stap2mm;   


float karInterval;
bool karGolven;


float karP = 0.002;//0.001;//0.0005; //0.00005;//0.00025;
float karI = 0;//0.005; //0.00005;//0.00025;
float karD = 0.0015;//0.0006;//-0.003;

float karBasis;
float karPcomp = 0;
float karDcomp = 0;




float plaatBegin = 0;

float nummers[100];// = {0.2, 0.3, 0.6, 0.68, 0.85}; //staat nu in staat.h
int hoeveelNummers = 0;





bool karMotorEnable = true;
float karMotorPos = 0;

float nieuwePos;
float karPos = KAR_HOK;
float karPosFilter = karPos;
float karPosFilterSlow = karPos;
float karPosMinimaal = karPos;;

float targetNummerPos = 0;
float sensorPos;
float karOffset = 0; // om te kijken wat de homing afwijking is




float armHoekRuw = analogRead(hoekSensor);
float armHoekRuwPrev;
float armHoekFilt;
float armHoekDiv;

float armHoek;
float armHoekSlow = armHoekRuw;
float armHoekOffset;


Interval naaldNaarVorenBewogen(1, MILLIS);






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



bool decelereerKar(){

  int richting = karSnelHeid > 0 ? 1 : -1;

  if(abs(karSnelHeid) < KAR_VERSNELLING){
    karSnelHeid = 0;
    return true;
  }

  versnelling = -KAR_VERSNELLING * richting;
  
  karPos += karSnelHeid + (versnelling / 2);

  karSnelHeid += versnelling;

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

  while(pos + NUMMER_TERUG_OPFSET >= nummers[nummer]){
    nummer++;
    if(nummer > hoeveelNummers - 1){
      gaNaarNummer(plaatBegin);
      return;
    }
  }

  if(nummers[nummer] > plaatBegin){
    gaNaarNummer(plaatBegin);
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

























void staatDingen(){
  
  if(staat == S_STOPPEN){
    if(naaldEraf() && decelereerKar()){
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

    if(armHoek < -500){//-800){//-1000)
      Serial.print("div; ");
      Serial.println(  (karOffset + karPos)  );
      karOffset -= KAR_HOME - karPos;
      karPos = KAR_HOME;

      karNoodStop();

      if(staat == S_HOMEN_VOOR_SPELEN){
        setStaat(S_NAAR_BEGIN_PLAAT);
      }else{
        setStaat(S_PARKEREN);
      }

      return;
    }
    
    return;
  }



  if(staat == S_PARKEREN){
    if(beweegKarNaarPos(KAR_HOK, KAR_MAX_SNELHEID)){
      setStaat(S_HOK);
    } 
    return;
  }



  if(staat == S_HOK){
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







  if(staat == S_NAAR_BEGIN_PLAAT){
    if(staatVeranderd.sinds() < 50){ // 10ms om wat shit te doen
      plaatBegin = 1000;
      return;
    }

    if(!plaatAanwezig){ // is er een plaat aanwezig
      
      float plaadDiaInch = (sensorPos / 25.4)*2;
      

      if(plaadDiaInch < 6){//kleiner dan 6" dan stoppen
        Serial.println("geen plaat? plaatDia: " + String(plaadDiaInch));
        stoppen();
        return;
      }
      
      if(plaadDiaInch < 8){// ongeveer 
        Serial.println("plaatDia: " + String(plaadDiaInch) + " : ±7\" ");
        setPlateauRpm(rpm45);
        plaatBegin = SINGLETJE_PLAAT_BEGIN;
        // return;
      
      
      }else if(plaadDiaInch < 11){ 
        Serial.println("plaatDia: " + String(plaadDiaInch) + " : ±10\" ");
        setPlateauRpm(rpm33);
        plaatBegin = TIEN_INCH_PLAAT_BEGIN;
      
      
      }else{
        Serial.println("plaatDia: " + String(plaadDiaInch) + " : ?\" ");
        plaatBegin = sensorPos;
        // setPlateauRpm(rpm33);
      }

      targetNummerPos = plaatBegin;
      setStaat(S_UITROLLEN_VOOR_SPELEN);
      return;
    }
    


    if(  beweegKarNaarPos(ELPEE_PLAAT_BEGIN,   KAR_MAX_SNELHEID)  ){
      plaatBegin = ELPEE_PLAAT_BEGIN;
      Serial.println("plaatDia: 12inch");
      setPlateauRpm(rpm33);

      setStaat(S_SPELEN);
      return;
    }
    
    return;
  }





  if(staat == S_UITROLLEN_VOOR_SPELEN){
    if(beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID)){ 
      setStaat(S_SPELEN);
      return;
    }
    return;
  }






  if(staat == S_SPELEN){
    // if(!decelereerKar()){return;}

    if(plaatAanwezigGefilterd < 0.5){ // is er nog een plaat aanwezig?
      stoppen();
      return;
    }
    
    if(naaldErop()){
      
      // float acu = 0;
      // acu += limieteerF(armHoek * -karP, -0.5, 0.5);
      // acu += limieteerF(armHoekDiv * -karD, -0.3, 0.3);
      // // karBasis += armHoek * -karI;
      // // acu += limieteerF(karBasis, -0.1, 0.1);

      // karPos += acu;


      // karPcomp = 0;
      // karPcomp += limieteerF(armHoek * karP, -0.5, 0.5);
      // karPcomp += limieteerF(armHoekDiv * karD, -0.3, 0.3);
      // karPos += limieteerF(armHoek * karI, -0.1, 0.1);
      
      nieuwePos = karPos + limieteerF(armHoek * -karP, -3, 3);
      // nieuwePos += limieteerF(armHoekDiv * karD, -0.3, 0.3);
      nieuwePos = limieteerF(nieuwePos, 0, plaatBegin);
      beweegKarNaarPos(nieuwePos, KAR_MAX_SNELHEID);
      
      if(karPos <= PLAAT_EINDE){
        Serial.println("kar heeft de limiet berijkt");
        stoppen();
        return;
      }

      if(!isOngeveer(karPos, karPosFilter, 2.5)){
        Serial.println("kar te ver terug gelopen / plaat te ver uit het midden");
        stoppen();
        return;
      }

      if(naaldNaarVorenBewogen.sinds() > 5000){
        Serial.println("kar te lang niet bewogen");
        stoppen();
        return;
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
        setStaat(S_SPELEN);
        return;
      }
      
      // karPos += limieteerF( targetNummerPos - karPos , -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);
    }
    return;
  }




  if(staat == S_DOOR_SPOELEN){
    if(naaldEraf()){
      beweegKarNaarPos(PLAAT_EINDE, KAR_MAX_SNELHEID/4);  
    }
  }

  if(staat == S_TERUG_SPOELEN){
    if(naaldEraf()){
      beweegKarNaarPos(plaatBegin, KAR_MAX_SNELHEID/4);
    }
  }

  if(staat == S_UITROLLEN_NA_SPOELEN){
    if(beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID/4)){ 
      setStaat(S_SPELEN);
      return;
    }
    return;
  }




  if(staat == S_PAUZE){
    if(naaldEraf()){
      if(staatVeranderd.sinds() > 3000){
        armHoekCalibreer();
      }
      beweegKarNaarPos(targetNummerPos, KAR_MAX_SNELHEID);
    }
    return;
  }




  if(staat == S_SCHOONMAAK){
    if(beweegKarNaarPos( SCHOONMAAK_PLEK,   KAR_MAX_SNELHEID)){
      naaldErop();
    }
    return;
  }


  if(staat == S_CALIBREER){
    if(beweegKarNaarPos( SCHOONMAAK_PLEK,   KAR_MAX_SNELHEID)){
      
    }
    return;
  }

}




































Interval karMotorInt(1000, MICROS);


bool karMotorUitvoeren(){
  
  karInterval = (micros() - karMotorInt.vorrigeVorrigeTijd) / 1000000.0;

  armHoekRuw = analogRead(hoekSensor);
  armHoekFilt += (armHoekRuw - armHoekFilt)/6;

  armHoekRuw = armHoekFilt;
  
  armHoekDiv = armHoekRuw - armHoekRuwPrev;
  armHoekRuwPrev = armHoekRuw;

  armHoekSlow += (armHoekRuw - armHoekSlow) / 100;

  armHoek = armHoekRuw - armHoekOffset;


  sensorPos = karPos - SENSOR_OFFSET;

  

  staatDingen();


  // karPos += limieteerF(armHoekDiv * -karD, -0.05, 0.05);//------------ Om oscilatie te voorkomen  

  // karDcomp = 0;
  karDcomp *= 0.999;
  karDcomp += limieteerF(armHoekDiv * -karD, -KAR_MAX_SNELHEID, KAR_MAX_SNELHEID);//------------ Om oscilatie te voorkomen
  // karDcomp = limieteerF(karDcomp, -3, 3);

  //  karMotorPos = (karPos + karOffset + limieteerF(armHoekDiv * -karD, -1, 1) )  *  mm2stap;

  karMotorPos = (karPos + karOffset + karDcomp)  *  mm2stap;
  // karMotorPos = (karPos + karOffset + karPcomp)  *  mm2stap;
  // karMotorPos = (karPos + karOffset)  *  mm2stap;


  if(karMotorEnable){
  // if(staat != S_HOK){

    pwmFase( sin(karMotorPos),  stapperAP, stapperAN, true);
    pwmFase( cos(karMotorPos),  stapperBP, stapperBN, true);
  
  }else{

    pwmFase( 0,  stapperAP, stapperAN, true);
    pwmFase( 0,  stapperBP, stapperBN, true);
  }



  if(staat == S_SPELEN  &&  isNaaldErop()){
    float div = karPos - karPosFilter;
    if(div < 0){
      karPosFilter += div / 1000;
            
    }

    if(karPos < karPosMinimaal){
      karPosMinimaal = karPos;
      naaldNaarVorenBewogen.reset();
    }
  }else{
    karPosFilter = karPos;
    karPosMinimaal = karPos;
    naaldNaarVorenBewogen.reset();
  }



  karMotorEnable = true;  




  if(karGolven){
    // Serial.print(karDcomp, 5);
    Serial.print(armHoekRuw);
    // Serial.print(',');
    
    // Serial.print(armHoekFilt);

    Serial.println();
  }

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






