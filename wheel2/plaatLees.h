



// float nummers[20] = {0.2, 0.3, 0.6, 0.68, 0.85}; //staat nu in staat.h
// int hoeveelNummers = 5;



float plaatLeesRuw;
float plaatLeesRuwOud;
float plaatLeesDiv;
float plaatLeesGefilterd;
float plaatLeesGefilterdOud;
float plaatLeesGefilterdBodem;
float plaatLeesDivTrack;




// bool plaatAanwezig = false;



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








#define plaatKnipperInterval 50

void plaatDetectie(){
  if(knip){
    plaatLeesGefilterdBodem += (plaatLeesRuw - plaatLeesGefilterdBodem) / 5;
  }

  knip =  ( millis()/plaatKnipperInterval ) % 2;
  
  if(knip){
    plaatLeesLedSetMilliAmp(20);
  }else{
    plaatLeesLedSetMilliAmp(0);
  }


  if(  plaatLeesRuw  >  plaatLeesGefilterd + 20     ^   laatsteKnipperRichting ){ //is er een knipper zichtbaar
    laatsteKnipperRichting = !laatsteKnipperRichting;

    vorrigeKnipperMeetLengte = laatsteKnipperMeetLengte;

    laatsteKnipperMeetLengte = millis() - laatsteKnipperMeet;   //bewaar de tijd van de flank
    laatsteKnipperMeet = millis();
  }
    

  
  plaatAanwezig = 
        (millis() - laatsteKnipperMeet) < plaatKnipperInterval* 1.2    
  &&    isOngeveer(laatsteKnipperMeetLengte,  plaatKnipperInterval,  plaatKnipperInterval*0.2)
  &&    isOngeveer(vorrigeKnipperMeetLengte,  plaatKnipperInterval,  plaatKnipperInterval*0.2);
}










#define MINIMALE_TRACK_AFSTAND 3//mm

int trackTresshold = 3200;
bool trackOnderTresh = true;


void scannenVoorTracks(){
  
  plaatLeesLedSetMilliAmp(10);

  // trackTresshold = plaatLeesGefilterdBodem + ((AMAX - plaatLeesGefilterdBodem) / 3);
  trackTresshold = (AMAX - plaatLeesGefilterdBodem) * 0.35;
  
  if(sensorPos < PLAAT_EINDE + 2){
    return;
  }
    
  if(plaatLeesDivTrack < trackTresshold && trackOnderTresh){
    trackOnderTresh = false;
  }

  if(plaatLeesDivTrack > trackTresshold && !trackOnderTresh){
    trackOnderTresh = true;

    if(hoeveelNummers == 0){
      Serial.print("begin plaat: ");
      Serial.println(sensorPos);
      nummers[hoeveelNummers] = sensorPos;
      hoeveelNummers++;    
    }else{
      if(sensorPos - nummers[hoeveelNummers] > MINIMALE_TRACK_AFSTAND){
        Serial.print("track op: ");
        Serial.println(sensorPos);
        nummers[hoeveelNummers] = sensorPos;
        hoeveelNummers++;
      }


    }

    
  }
  
}




















Interval plaatLeesInt(5000, MICROS);

void plaatLeesFunc(){
  if(plaatLeesInt.loop()){
    
    plaatLeesRuw = analogRead(plaatLees);

    plaatLeesGefilterd += (plaatLeesRuw - plaatLeesGefilterd) / 5;
    
    plaatLeesDiv = plaatLeesGefilterd - plaatLeesGefilterdOud;
    // plaatLeesDiv = plaatLeesRuw - plaatLeesRuwOud;

    if(plaatLeesDiv > 0){//                     vermeer het effect van omhooggaande flanken, om nummer te vinden
      plaatLeesDivTrack += plaatLeesDiv;
    }else{
      plaatLeesDivTrack = 0;
    }

    // plaatLeesDivTrack = plaatLeesRuwOud - plaatLeesRuw;

    plaatLeesGefilterdOud = plaatLeesGefilterd;
    // plaatLeesRuwOud = plaatLeesRuw;




    if(staat == S_HOK){
      hoeveelNummers = 0;
      nummers[hoeveelNummers] = 1;
      plaatLeesLedSetMilliAmp(0);
      return;
    }
    


    if(staat == S_NAAR_BEGIN_PLAAT){//--------------------------------                   TRACKS LEZEN

      scannenVoorTracks();
      return;
    }





    //---------------------------------------------------------------                    PLAAT DETECTIE

    plaatDetectie();

    if(staat == S_PLAAT_AANWEZIG){
      if(staatVeranderd.sinds() > 1000){
        if(plaatAanwezig){
          armHoekCalibreer();
          setStaat(S_NAAR_BEGIN_PLAAT);
          plateauDraaien();
        }else{
          stoppen();
        }
      }
    }
    


  }
}









