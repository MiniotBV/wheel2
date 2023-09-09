
bool plaatLeesGolven = false;



float plaatLeesRuw;
float plaatLeesRuwPrev;
float plaatLeesRuwDiv;

float plaatLeesGefilterd;
float plaatLeesGefilterdOud;

float plaatLeesDiv;
float plaatLeesDivOud;

float plaatLeesDivDiv;
float plaatLeesDivDivOud;

float plaatLeesPre;
float plaatLeesOmhoog;
float plaatLeesOmlaag;

float plaatLeesTrack;




unsigned int laatsteKnipperMeet;
unsigned int laatsteKnipperMeetLengte;
unsigned int vorrigeKnipperMeetLengte;
bool laatsteKnipperRichting;
bool knip;






int volt2pwm(float volt){
  return (volt * PMAX) / 3.3;
}



void plaatLeesLedSetMilliAmp(float amp){
  amp /= 1000.0;
  pwmWrite(plaatLeesLed, volt2pwm(1 + (100 * amp)));  //100ohm + 1volt led drop
}




void plaatLeesInit(){
  setPwm(plaatLeesLed);
  plaatLeesLedSetMilliAmp(0);//10mA
}








// #define plaatKnipperInterval 50
#define plaatDetectieTreshold 100

void plaatDetectie(){
  if(knip){
    plaatAanwezig = plaatLeesRuwDiv < -plaatDetectieTreshold;
  }else{
    plaatAanwezig = plaatLeesRuwDiv > plaatDetectieTreshold;
  }

  plaatAanwezigGefilterd += (plaatAanwezig - plaatAanwezigGefilterd) / 10;

  knip = !knip;

  if(knip){
    plaatLeesLedSetMilliAmp(10);
  }else{
    plaatLeesLedSetMilliAmp(0);
  }
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
  
  // plaatLeesLedSetMilliAmp(10);

  // trackTresshold = 50;
  
  if(sensorPos < PLAAT_EINDE + 2){
    return;
  }

  if(plaatLeesOmhoog < trackTresshold && trackOnderTresh){  trackOnderTresh = false; }

  if(plaatLeesOmhoog > trackTresshold && !trackOnderTresh){
    trackOnderTresh = true;

    nieuwNummer(sensorPos - trackOffset);
  }
  
}




















Interval plaatLeesInt(5000, MICROS);

void plaatLeesFunc(){
  if(plaatLeesInt.loop()){
    
    if(staat == S_HOK){
      hoeveelNummers = 0;
      nummers[hoeveelNummers] = 1;
      plaatLeesLedSetMilliAmp(0);
      return;
    }



    plaatLeesRuwPrev = plaatLeesRuw;
    plaatLeesRuw = analogRead(plaatLees);
    plaatLeesRuwDiv = plaatLeesRuw - plaatLeesRuwPrev;

    if(knip){
      plaatLeesGefilterd += (plaatLeesRuw - plaatLeesGefilterd) / 5;

      plaatLeesDiv = plaatLeesGefilterd - plaatLeesGefilterdOud;
      plaatLeesGefilterdOud = plaatLeesGefilterd;

      // if(plaatLeesDiv > 0) plaatLeesTrack += plaatLeesDiv; else plaatLeesTrack = 0;

      plaatLeesDivDiv = plaatLeesDiv - plaatLeesDivOud;
      plaatLeesDivOud = plaatLeesDiv;

      plaatLeesPre += (plaatLeesDivDiv - plaatLeesPre)/5;

      if(plaatLeesPre > 0) plaatLeesOmhoog += plaatLeesPre; else plaatLeesOmhoog = 0;
      if(plaatLeesPre < 0) plaatLeesOmlaag += plaatLeesPre; else plaatLeesOmlaag = 0;
    }

    plaatDetectie();




    if(plaatLeesGolven){
      Serial.print(plaatLeesRuw);
      // Serial.print(", ");
      // Serial.print(plaatLeesRuwDiv);

      // Serial.print(", ");
      // Serial.print(plaatAanwezig*1000);

      Serial.print(", ");
      Serial.print(plaatLeesGefilterd);

      Serial.print(", ");
      Serial.print(plaatLeesDiv);

      Serial.print(", ");
      Serial.print(plaatLeesDivDiv);



      Serial.print(", ");
      Serial.print(plaatLeesPre);
      Serial.print(", ");
      Serial.print(plaatLeesOmhoog);
      Serial.print(", ");
      Serial.print(plaatLeesOmlaag);

      Serial.println();
    }




    


    if(staat == S_NAAR_BEGIN_PLAAT){//--------------------------------                   TRACKS LEZEN
      scannenVoorTracks();
      return;
    }
    
  }
}












