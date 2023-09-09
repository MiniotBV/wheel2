
bool plaatLeesGolven = false;



float plaatLeesRuw;
float plaatLeesRuwPrev;
float plaatLeesRuwDiv;

float plaatLeesAbsDiv;
float plaatLeesWaarde;

float plaatLeesMax;
float plaatLeesMin;

float plaatLeesGefilterd;
float plaatLeesGefilterdOud;

float plaatLeesDiv;
float plaatLeesDivOud;

float plaatLeesDivDiv;

float plaatLeesPre;
float plaatLeesOmhoog;
float plaatLeesOmlaag;

float plaatLeesTrack;




unsigned int laatsteKnipperMeet;
unsigned int laatsteKnipperMeetLengte;
unsigned int vorrigeKnipperMeetLengte;
bool laatsteKnipperRichting;
bool knip;




float plaatLeesStroom = 10;
float plaatLedStroomP = 0.001;
float plaatSensorTarget = 1500;





int volt2pwm(float volt){
  return (volt * PMAX) / 3.3;
}



void plaatLeesLedSetMilliAmp(float amp){
  amp /= 1000.0;
  pwmWrite(plaatLeesLed, volt2pwm(1 + (100 * amp)));  //100ohm + 1volt led drop
}

void plaatLeesLeduit(){
  pwmWrite(plaatLeesLed, 0);  //100ohm + 1volt led drop
}




void plaatLeesInit(){
  setPwm(plaatLeesLed);
  plaatLeesLedSetMilliAmp(0);//10mA
}








// #define plaatKnipperInterval 50
#define plaatDetectieTreshold 100

void plaatDetectie(){
  
  plaatAanwezig = plaatLeesAbsDiv > plaatDetectieTreshold;//als er genoeg amplitude is is er een plaat

  plaatAanwezigGefilterd += (plaatAanwezig - plaatAanwezigGefilterd) / 10;
}








void nieuwNummer(float pos){
  if(hoeveelNummers == 0){
    Serial.println("einde plaat: " + String(pos));
  }else{
    float afstand = pos - nummers[hoeveelNummers-1];
    if(afstand < 2){return;}
    Serial.println("track op: " + String(pos));
    // Serial.println("afstand tot vorrige track: " + String(afstand));
  }

  nummers[hoeveelNummers] = pos;
  hoeveelNummers++;  
}




#define MINIMALE_TRACK_AFSTAND 3//mm

int trackTresshold = 50;
bool trackOnderTresh = true;


void scannenVoorTracks(){
  
  if(sensorPos < PLAAT_EINDE + 2  ||  staat != S_NAAR_BEGIN_PLAAT){
    return;
  }

  if(plaatLeesOmhoog < trackTresshold && trackOnderTresh){  trackOnderTresh = false; }

  if(plaatLeesOmhoog > trackTresshold && !trackOnderTresh){
    trackOnderTresh = true;

    nieuwNummer(sensorPos - trackOffset);
  }


  if(plaatLeesGolven){

    Serial.print(plaatLeesWaarde);

    Serial.print(", ");
    Serial.print(plaatLeesAbsDiv);

    Serial.print(", ");
    Serial.print(plaatLeesMax);
    Serial.print(", ");
    Serial.print(plaatLeesMin);

    Serial.print(", ");
    Serial.print(plaatLeesStroom * 100);


    Serial.println();
  }
  
}




















Interval plaatLeesInt(10000, MICROS);

void plaatLeesFunc(){
  if(plaatLeesInt.loop()){
    
    if(staat == S_HOK){ // als de kar in hok is, plaat scanner uitzetten
      hoeveelNummers = 0;
      nummers[hoeveelNummers] = 1;
      plaatLeesLeduit();
      return;
    }

    // if(sensorPos < PLAAT_EINDE){
    //   return;      
    // }



    plaatLeesRuwPrev = plaatLeesRuw;
    plaatLeesRuw = analogRead(plaatLees);
    plaatLeesRuwDiv = plaatLeesRuw - plaatLeesRuwPrev;


    plaatLeesAbsDiv = abs(plaatLeesRuwDiv);

    plaatLeesWaarde = abs(plaatLeesAbsDiv - plaatSensorTarget);


    plaatDetectie();
    

    if(knip){

      float ledStroomP = (plaatSensorTarget - plaatLeesAbsDiv) * plaatLedStroomP;
      plaatLeesStroom += limieteerF(ledStroomP,  -1, 1);

      plaatLeesStroom = limieteerF(plaatLeesStroom, 5, 20);

      plaatLeesMax = plaatLeesRuw;
      
      plaatLeesGefilterd += (plaatLeesAbsDiv - plaatLeesGefilterd) / 5;
      // plaatLeesGefilterd += (plaatLeesRuw - plaatLeesGefilterd) / 5;

      plaatLeesDiv = plaatLeesGefilterd - plaatLeesGefilterdOud;
      plaatLeesGefilterdOud = plaatLeesGefilterd;

      // if(plaatLeesDiv > 0) plaatLeesTrack += plaatLeesDiv; else plaatLeesTrack = 0;

      plaatLeesDivDiv = plaatLeesDiv - plaatLeesDivOud;
      plaatLeesDivOud = plaatLeesDiv;

      plaatLeesPre += (plaatLeesDivDiv - plaatLeesPre)/5;

      if(plaatLeesPre > 0) plaatLeesOmhoog += plaatLeesPre; else plaatLeesOmhoog = 0;
      if(plaatLeesPre < 0) plaatLeesOmlaag += plaatLeesPre; else plaatLeesOmlaag = 0;




      scannenVoorTracks();//--------------------------------                   TRACKS LEZEN
    
    
    }else{
      plaatLeesMin = plaatLeesRuw;
    }

    


    knip = !knip;

    if(knip){
      plaatLeesLedSetMilliAmp(plaatLeesStroom);
    }else{
      plaatLeesLeduit();
    }

    
  }
}












